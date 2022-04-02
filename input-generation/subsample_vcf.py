import sys
import random

prob = float(sys.argv[2])
random.seed(1337)

with open(sys.argv[1], 'r') as f:
    for line in f:
        if line[0] == '#':
            sys.stdout.write(line)
            continue

        if random.random() < prob:
            sys.stdout.write(line)
