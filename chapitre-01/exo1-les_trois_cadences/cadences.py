#!/usr/bin/env python3
"""
Exercice 1 : Les trois cadences
ANI-IA 5089 — Coder pour la VR, l'XR et l'AR III
"""

def main():
    system_latency = 8.0  # ms
    frequencies = [72, 90, 120]
    
    print("Fréquence | Durée totale | Système | Reste pour le code")
    print("-" * 55)
    for freq in frequencies:
        frame_time = 1000.0 / freq
        code_time = frame_time - system_latency
        print(f"{freq:3d} Hz    | {frame_time:5.1f} ms      | {system_latency:4.1f} ms  | {code_time:5.1f} ms")

if __name__ == "__main__":
    main()
