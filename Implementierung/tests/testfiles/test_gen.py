from os import sep, system
import numpy as np
import sys

np.set_printoptions(threshold = sys.maxsize)


f = open("data_500_000_non-uni-5", "w+")

# Random Generation

a = np.random.random(500_000)
f.write(str(a / sum(a)).lstrip("[").rstrip("]"))

# Uniform Generation
# n = 100000
# a = [1/n] * n
# f.write(' '.join(map(str, a)))

f.close()