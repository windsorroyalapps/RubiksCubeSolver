package com.windsorroyal.rubikscubesolver

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Button
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import com.windsorroyal.rubikscubesolver.ui.theme.RubiksCubeSolverTheme

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()

        // Create a 3x3 by default
        NativeSolver.create(3)

        setContent {
            RubiksCubeSolverTheme {
                var status by remember { mutableStateOf("3x3 ready — solved = ${NativeSolver.isSolved()}") }
                var dump by remember { mutableStateOf(NativeSolver.dump()) }

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
                            text = "RubiksCubeSolver",
                            style = MaterialTheme.typography.headlineMedium
                        )
                        Text(
                            text = "Android/BMW style native engine",
                            style = MaterialTheme.typography.bodyMedium
                        )

                        Spacer(Modifier.height(16.dp))

                        Text(text = status)

                        Spacer(Modifier.height(12.dp))

                        Button(onClick = {
                            // Scramble with a classic sequence
                            NativeSolver.applyNotation("R U R' U' R' F R2 U' R' U' R U R' F'")
                            status = "Scrambled — solved = ${NativeSolver.isSolved()}"
                            dump = NativeSolver.dump()
                        }) {
                            Text("Scramble (T-perm)")
                        }

                        Button(onClick = {
                            // Apply the inverse to solve it back (for demo)
                            NativeSolver.applyNotation("F R U' R' U R U R2 F' R U R U' R'")
                            status = "Applied inverse — solved = ${NativeSolver.isSolved()}"
                            dump = NativeSolver.dump()
                        }) {
                            Text("Solve (inverse)")
                        }

                        Button(onClick = {
                            NativeSolver.create(3)
                            status = "Reset 3x3 — solved = ${NativeSolver.isSolved()}"
                            dump = NativeSolver.dump()
                        }) {
                            Text("Reset 3x3")
                        }

                        Button(onClick = {
                            NativeSolver.create(5)
                            status = "Created 5x5 — solved = ${NativeSolver.isSolved()}"
                            dump = NativeSolver.dump()
                        }) {
                            Text("Create 5x5")
                        }

                        Spacer(Modifier.height(16.dp))

                        Text(
                            text = dump,
                            style = MaterialTheme.typography.bodySmall
                        )
                    }
                }
            }
        }
    }
}
