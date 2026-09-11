package com.windsorroyal.rubikscubesolver

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Button
import androidx.compose.material3.FilterChip
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import com.windsorroyal.rubikscubesolver.billing.BillingManager
import com.windsorroyal.rubikscubesolver.billing.PremiumStore
import com.windsorroyal.rubikscubesolver.ui.theme.RubiksCubeSolverTheme
import kotlin.random.Random

class MainActivity : ComponentActivity() {

    private lateinit var premiumStore: PremiumStore
    private lateinit var billingManager: BillingManager

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()

        premiumStore = PremiumStore(this)
        billingManager = BillingManager(this, premiumStore)
        billingManager.startConnection()

        NativeSolver.create(3)

        setContent {
            RubiksCubeSolverTheme {
                val premium by billingManager.premiumOwned.collectAsState()
                val billingMsg by billingManager.statusMessage.collectAsState()

                var cubeSize by remember { mutableIntStateOf(3) }
                var status by remember {
                    mutableStateOf("3×3 ready — solved = ${NativeSolver.isSolved()}")
                }
                var dump by remember { mutableStateOf(NativeSolver.dump()) }
                var lastSolution by remember { mutableStateOf("") }
                var boundReport by remember { mutableStateOf("") }
                var showPaywall by remember { mutableStateOf(false) }

                fun selectSize(size: Int) {
                    if (!premiumStore.canUseSize(size)) {
                        showPaywall = true
                        status = "Premium required for ${size}×${size}"
                        return
                    }
                    showPaywall = false
                    cubeSize = size
                    NativeSolver.create(size)
                    status = "${size}×${size} ready — solved = ${NativeSolver.isSolved()}"
                    dump = NativeSolver.dump()
                    lastSolution = ""
                    boundReport = ""
                }

                fun scrambleCurrent() {
                    if (cubeSize == 3) {
                        NativeSolver.applyNotation(
                            "R U R' U' R' F R2 U' R' U' R U R' F'"
                        )
                    } else {
                        // Short random-ish SiGN-ish sequence; fall back to repeated T-perm
                        val faces = listOf("U", "D", "F", "B", "L", "R")
                        val parts = mutableListOf<String>()
                        repeat(8 + cubeSize) {
                            val f = faces[Random.nextInt(faces.size)]
                            val depth = if (Random.nextBoolean()) "" else "2"
                            val turn = listOf("", "'", "2").random()
                            // e.g. R, R', 2R, 2R2 — native applyNotation accepts SiGN
                            parts += if (depth.isEmpty()) "$f$turn" else "$depth$f$turn"
                        }
                        try {
                            NativeSolver.applyNotation(parts.joinToString(" "))
                        } catch (_: Throwable) {
                            repeat(cubeSize) {
                                NativeSolver.applyNotation(
                                    "R U R' U' R' F R2 U' R' U' R U R' F'"
                                )
                            }
                        }
                    }
                    status = "Scrambled ${cubeSize}×${cubeSize} — solved = ${NativeSolver.isSolved()}"
                    dump = NativeSolver.dump()
                    lastSolution = ""
                    boundReport = ""
                }

                Scaffold(modifier = Modifier.fillMaxSize()) { innerPadding ->
                    Column(
                        modifier = Modifier
                            .padding(innerPadding)
                            .padding(16.dp)
                            .fillMaxSize()
                            .verticalScroll(rememberScrollState()),
                        verticalArrangement = Arrangement.Top,
                        horizontalAlignment = Alignment.CenterHorizontally
                    ) {
                        Text(
                            text = "Cube Solver",
                            style = MaterialTheme.typography.headlineMedium
                        )
                        Text(
                            text = "Free 3×3 · Premium unlocks 4×4 & 5×5",
                            style = MaterialTheme.typography.bodyMedium
                        )
                        Text(
                            text = "Offline native CFOP + reduction engine",
                            style = MaterialTheme.typography.bodySmall
                        )

                        Spacer(modifier = Modifier.height(16.dp))

                        Row(
                            horizontalArrangement = Arrangement.spacedBy(8.dp),
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            listOf(3, 4, 5).forEach { size ->
                                val locked = !premiumStore.canUseSize(size) && !premium
                                FilterChip(
                                    selected = cubeSize == size && !locked,
                                    onClick = { selectSize(size) },
                                    label = {
                                        Text(
                                            if (locked) "${size}×${size} 🔒"
                                            else "${size}×${size}"
                                        )
                                    }
                                )
                            }
                        }

                        if (showPaywall || (!premium && cubeSize > 3)) {
                            Spacer(modifier = Modifier.height(8.dp))
                            Text(
                                text = "Larger cubes need Premium. Unlock once via Google Play.",
                                style = MaterialTheme.typography.bodySmall
                            )
                            Button(onClick = {
                                val ok = billingManager.launchBillingFlow(this@MainActivity)
                                if (!ok) {
                                    status = billingMsg
                                        ?: "Unable to start purchase — is Play Store available?"
                                }
                            }) {
                                Text("Unlock Premium")
                            }
                        }

                        Spacer(modifier = Modifier.height(12.dp))

                        Text(text = status)
                        if (!billingMsg.isNullOrBlank()) {
                            Text(
                                text = billingMsg!!,
                                style = MaterialTheme.typography.bodySmall
                            )
                        }
                        if (premium) {
                            Text(
                                text = "Premium active",
                                style = MaterialTheme.typography.labelLarge,
                                color = MaterialTheme.colorScheme.primary
                            )
                        }

                        if (lastSolution.isNotEmpty()) {
                            Spacer(modifier = Modifier.height(8.dp))
                            Text(
                                text = "Solution: $lastSolution",
                                style = MaterialTheme.typography.bodySmall,
                                modifier = Modifier.fillMaxWidth()
                            )
                        }
                        if (boundReport.isNotEmpty()) {
                            Text(
                                text = "Bounds: $boundReport",
                                style = MaterialTheme.typography.bodySmall,
                                modifier = Modifier.fillMaxWidth()
                            )
                        }

                        Spacer(modifier = Modifier.height(12.dp))

                        Button(onClick = { scrambleCurrent() }) {
                            Text(if (cubeSize == 3) "Scramble (T-perm)" else "Scramble")
                        }

                        Button(onClick = {
                            val sol = NativeSolver.solve()
                            lastSolution = sol.ifEmpty { "(empty / not fully implemented yet)" }
                            status =
                                "Solved ${cubeSize}×${cubeSize} — solved = ${NativeSolver.isSolved()}"
                            dump = NativeSolver.dump()
                            boundReport = if (cubeSize > 3) {
                                NativeSolver.boundReport().ifBlank { "(no bound report)" }
                            } else {
                                ""
                            }
                        }) {
                            Text("Solve")
                        }

                        Button(onClick = {
                            selectSize(cubeSize)
                            status =
                                "Reset ${cubeSize}×${cubeSize} — solved = ${NativeSolver.isSolved()}"
                        }) {
                            Text("Reset")
                        }

                        OutlinedButton(onClick = {
                            billingManager.restorePurchases()
                            status = "Restore requested"
                            // Re-evaluate size lock after restore settles
                            if (premiumStore.isPremium() && showPaywall) {
                                showPaywall = false
                            }
                        }) {
                            Text("Restore purchases")
                        }

                        Spacer(modifier = Modifier.height(16.dp))

                        Text(
                            text = dump,
                            style = MaterialTheme.typography.bodySmall,
                            modifier = Modifier.fillMaxWidth()
                        )
                    }
                }
            }
        }
    }

    override fun onDestroy() {
        billingManager.disconnect()
        super.onDestroy()
    }
}
