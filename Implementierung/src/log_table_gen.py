import numpy as np
import math

n  = 14

steps = 1 << n

x = 1.0 + (1.0 / (1 << (n + 1)))
step_size = 1.0 / steps

list = []

for i in range(steps >> 2):
    tmp = []
    for j in range(4):
        tmp.append(math.log2(x))
        x += step_size
    list.append(tmp)

f = open("log_const.c", "a+")

string = "{\n"

for i in list:
    string += str(i).lstrip('[').rstrip(']') + ",\n"

string = string.rstrip().rstrip(',') + '\n'
f.write(string + "};\n")

f.close()
