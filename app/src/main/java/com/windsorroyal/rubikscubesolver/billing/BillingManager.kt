package com.windsorroyal.rubikscubesolver.billing

import android.app.Activity
import android.content.Context
import android.util.Log
import com.android.billingclient.api.AcknowledgePurchaseParams
import com.android.billingclient.api.BillingClient
import com.android.billingclient.api.BillingClientStateListener
import com.android.billingclient.api.BillingFlowParams
import com.android.billingclient.api.BillingResult
import com.android.billingclient.api.PendingPurchasesParams
import com.android.billingclient.api.ProductDetails
import com.android.billingclient.api.Purchase
import com.android.billingclient.api.PurchasesUpdatedListener
import com.android.billingclient.api.QueryProductDetailsParams
import com.android.billingclient.api.QueryPurchasesParams
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow

/**
 * Thin Play Billing Library wrapper for the non-consumable [PRODUCT_ID] unlock.
 * Handles missing Play Store / Billing gracefully (connection failure → not owned).
 */
class BillingManager(
    context: Context,
    private val premiumStore: PremiumStore,
) : PurchasesUpdatedListener {

    companion object {
        const val PRODUCT_ID = "premium_unlock"
        private const val TAG = "BillingManager"
    }

    private val appContext = context.applicationContext

    private val _premiumOwned = MutableStateFlow(premiumStore.isPremium())
    val premiumOwned: StateFlow<Boolean> = _premiumOwned.asStateFlow()

    private val _productDetails = MutableStateFlow<ProductDetails?>(null)
    val productDetails: StateFlow<ProductDetails?> = _productDetails.asStateFlow()

    private val _billingAvailable = MutableStateFlow(false)
    val billingAvailable: StateFlow<Boolean> = _billingAvailable.asStateFlow()

    private val _statusMessage = MutableStateFlow<String?>(null)
    val statusMessage: StateFlow<String?> = _statusMessage.asStateFlow()

    private var billingClient: BillingClient? = null

    fun startConnection() {
        if (billingClient != null) return
        val client = try {
            BillingClient.newBuilder(appContext)
                .setListener(this)
                .enablePendingPurchases(
                    PendingPurchasesParams.newBuilder()
                        .enableOneTimeProducts()
                        .build()
                )
                .build()
        } catch (t: Throwable) {
            Log.w(TAG, "BillingClient unavailable", t)
            _billingAvailable.value = false
            _statusMessage.value = "Play Billing unavailable on this device"
            syncLocalPremium()
            return
        }
        billingClient = client
        client.startConnection(object : BillingClientStateListener {
            override fun onBillingSetupFinished(result: BillingResult) {
                if (result.responseCode == BillingClient.BillingResponseCode.OK) {
                    _billingAvailable.value = true
                    _statusMessage.value = null
                    queryProductDetails()
                    restorePurchases()
                } else {
                    _billingAvailable.value = false
                    _statusMessage.value =
                        "Play Store not ready (${result.responseCode})"
                    syncLocalPremium()
                }
            }

            override fun onBillingServiceDisconnected() {
                _billingAvailable.value = false
            }
        })
    }

    fun disconnect() {
        try {
            billingClient?.endConnection()
        } catch (_: Throwable) {
        }
        billingClient = null
        _billingAvailable.value = false
    }

    fun queryProductDetails() {
        val client = billingClient ?: return
        val product = QueryProductDetailsParams.Product.newBuilder()
            .setProductId(PRODUCT_ID)
            .setProductType(BillingClient.ProductType.INAPP)
            .build()
        val params = QueryProductDetailsParams.newBuilder()
            .setProductList(listOf(product))
            .build()
        client.queryProductDetailsAsync(params) { result, detailsResult ->
            if (result.responseCode == BillingClient.BillingResponseCode.OK) {
                _productDetails.value = detailsResult.productDetailsList.firstOrNull()
            } else {
                Log.w(TAG, "queryProductDetails failed: ${result.responseCode}")
            }
        }
    }

    fun launchBillingFlow(activity: Activity): Boolean {
        val client = billingClient
        val details = _productDetails.value
        if (client == null || details == null || !_billingAvailable.value) {
            _statusMessage.value = "Premium purchase unavailable (no Play Store product)"
            return false
        }
        val productParams = BillingFlowParams.ProductDetailsParams.newBuilder()
            .setProductDetails(details)
            .build()
        val flowParams = BillingFlowParams.newBuilder()
            .setProductDetailsParamsList(listOf(productParams))
            .build()
        val result = client.launchBillingFlow(activity, flowParams)
        return result.responseCode == BillingClient.BillingResponseCode.OK
    }

    fun restorePurchases() {
        val client = billingClient
        if (client == null || !_billingAvailable.value) {
            syncLocalPremium()
            _statusMessage.value = "Restore skipped — Billing not connected"
            return
        }
        val params = QueryPurchasesParams.newBuilder()
            .setProductType(BillingClient.ProductType.INAPP)
            .build()
        client.queryPurchasesAsync(params) { result, purchases ->
            if (result.responseCode != BillingClient.BillingResponseCode.OK) {
                _statusMessage.value = "Restore failed (${result.responseCode})"
                syncLocalPremium()
                return@queryPurchasesAsync
            }
            var owned = false
            for (purchase in purchases) {
                if (purchase.products.contains(PRODUCT_ID) &&
                    purchase.purchaseState == Purchase.PurchaseState.PURCHASED
                ) {
                    owned = true
                    acknowledgeIfNeeded(purchase)
                }
            }
            premiumStore.setPremiumLocal(owned || premiumStore.isDebugBypass())
            _premiumOwned.value = premiumStore.isPremium()
            _statusMessage.value = if (owned) "Purchases restored" else "No premium purchase found"
        }
    }

    override fun onPurchasesUpdated(result: BillingResult, purchases: MutableList<Purchase>?) {
        when (result.responseCode) {
            BillingClient.BillingResponseCode.OK -> {
                purchases?.forEach { purchase ->
                    if (purchase.products.contains(PRODUCT_ID) &&
                        purchase.purchaseState == Purchase.PurchaseState.PURCHASED
                    ) {
                        acknowledgeIfNeeded(purchase)
                        premiumStore.setPremiumLocal(true)
                        _premiumOwned.value = true
                        _statusMessage.value = "Premium unlocked"
                    }
                }
            }
            BillingClient.BillingResponseCode.USER_CANCELED -> {
                _statusMessage.value = "Purchase canceled"
            }
            else -> {
                _statusMessage.value = "Purchase error (${result.responseCode})"
            }
        }
    }

    private fun acknowledgeIfNeeded(purchase: Purchase) {
        if (purchase.isAcknowledged) return
        val client = billingClient ?: return
        val params = AcknowledgePurchaseParams.newBuilder()
            .setPurchaseToken(purchase.purchaseToken)
            .build()
        client.acknowledgePurchase(params) { ackResult ->
            if (ackResult.responseCode != BillingClient.BillingResponseCode.OK) {
                Log.w(TAG, "acknowledge failed: ${ackResult.responseCode}")
            }
        }
    }

    private fun syncLocalPremium() {
        _premiumOwned.value = premiumStore.isPremium()
    }
}
