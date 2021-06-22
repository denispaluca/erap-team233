from os import sep, system
import numpy as np
import sys

np.set_printoptions(threshold = sys.maxsize)


f = open("data_10_000_non-uni", "w+")

# Random Generation

a = np.random.random(10000)
f.write(str(a / sum(a)).lstrip("[").rstrip("]"));

# Uniform Generation
# n = 1000000
# a = [1/n] * n
# f.write(' '.join(map(str, a)))

f.close()