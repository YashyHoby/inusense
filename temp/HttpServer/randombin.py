import random

with open("random_bytes.bin", "wb") as f:
    f.write(bytes(random.randint(0, 255) for _ in range(1024)))