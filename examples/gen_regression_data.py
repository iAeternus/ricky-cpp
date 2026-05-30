import math
import random
import os

random.seed(42)

N_SAMPLES = 300
OUTPUT_DIR = os.path.join(os.path.dirname(__file__), "data")
os.makedirs(OUTPUT_DIR, exist_ok=True)

with open(os.path.join(OUTPUT_DIR, "regression.csv"), "w") as f:
    for i in range(N_SAMPLES):
        x = -3.0 + 6.0 * i / (N_SAMPLES - 1)
        noise = random.gauss(0, 0.1)
        y = math.sin(2 * math.pi * x) + noise
        f.write(f"{x},{y}\n")

print(f"Generated {N_SAMPLES} regression samples -> {OUTPUT_DIR}/regression.csv")
