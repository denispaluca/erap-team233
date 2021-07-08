import math

n  = 16

steps = 1 << n

x = 1.0
step_size = 1.0 / steps

list = []

# Why do we start with 10 but than add pow(2, -(n+1)) for once ?
# The reason behind it that we start with 1.0 because we want %100 accuracy for the numbers
# which are power of 2 , e.g 2, 4, 8, 16 ....
# on the other hand we ignore last (23 - n) bits of our number, and we save the same value for all combinations of ignored bits.
# if we save this value at where all the ignored bits are 0, than the mistake is going to be higher. So we save the middle value for these numbers

# We can show that by using integration in a ceratin interval [a,b] with a < b
# Let the all values at this interval fixed and equals to log_2(x)
# then the function f(x) = (∫|log_2(x) - log_2(t)|.dt from a to b) gives the total mistake on this interval for saved value log_2(x)
# Function f is in interval [a, b] minimum at (a + b)/2 (because f'((a + b)/2) = 0) and maximum at a in
# so if we assume we have 7 ignored bits, the middle value of ...0000000 and ...1111111 is ...1000000
# so that is aquivalent to summing 2^(-n - 1)
# in our case we took n = 16, so we save all values x != 1.0 as log_2(x + 2^-17)

begin_zero = True

for i in range(steps >> 2):
    tmp = []
    for j in range(4):
        tmp.append(math.log2(x))
        x += step_size
        if begin_zero:
            x += pow(2, -(n + 1))
            begin_zero = False
    list.append(tmp)

f = open("log2_consts.c", "a+")

string = "{\n"

for i in list:
    string += str(i).lstrip('[').rstrip(']') + ",\n"

string = string.rstrip().rstrip(',') + '\n'
f.write(string + "};\n")

f.close()
