package com.windsorroyal.rubikscubesolver.billing

import android.content.Context
import com.windsorroyal.rubikscubesolver.BuildConfig

/**
 * Local entitlement cache for premium unlock.
 *
 * Free tier: cube size == 3 only.
 * Premium: SharedPreferences flag set after Play purchase / restore,
 * or debug bypass via [BuildConfig.DEBUG_PREMIUM] or debug preference.
 */
class PremiumStore(context: Context) {

    companion object {
        private const val PREFS = "premium_store"
        private const val KEY_PREMIUM = "premium_owned"
        private const val KEY_DEBUG_UNLOCK = "debug_developer_unlock"
        const val FREE_MAX_SIZE = 3
    }

    private val prefs = context.applicationContext.getSharedPreferences(PREFS, Context.MODE_PRIVATE)

    fun isDebugBypass(): Boolean {
        if (BuildConfig.DEBUG_PREMIUM) return true
        if (BuildConfig.DEBUG && prefs.getBoolean(KEY_DEBUG_UNLOCK, false)) return true
        return false
    }

    fun isPremium(): Boolean = isDebugBypass() || prefs.getBoolean(KEY_PREMIUM, false)

    fun setPremiumLocal(owned: Boolean) {
        prefs.edit().putBoolean(KEY_PREMIUM, owned).apply()
    }

    /** Debug builds only — developer unlock toggle. */
    fun setDebugDeveloperUnlock(enabled: Boolean) {
        if (!BuildConfig.DEBUG) return
        prefs.edit().putBoolean(KEY_DEBUG_UNLOCK, enabled).apply()
    }

    fun canUseSize(size: Int): Boolean {
        if (size <= FREE_MAX_SIZE) return true
        return isPremium()
    }
}
