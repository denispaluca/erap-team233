import math

n  = 16

steps = 1 << n

x = 1.0
step_size = 1.0 / steps

list = []

for i in range(steps >> 2):
    tmp = []
    for j in range(4):
        tmp.append(math.log2(x))
        x += step_size
    list.append(tmp)

f = open("log2_consts.c", "a+")

string = "{\n"

for i in list:
    for j in i:
        string += str(float(j)) + ", "
    string += "\n"

string = string.rstrip().rstrip(',') + '\n'
f.write(string + "};\n")

f.close()
