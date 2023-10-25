import matplotlib.pyplot as plt

x = []
y = []

file = open('outgoing.txt', 'r')
lines = file.readlines()

for line in lines:
    splits = line.split()
    x.append(splits[0])
    y.append(splits[1])

plt.bar(x, y)
plt.show()