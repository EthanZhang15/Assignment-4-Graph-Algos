import matplotlib.pyplot as plt

x = []
y = []

file = open('road-NYOutgoing.txt', 'r')
lines = file.readlines()

for line in lines:
    splits = line.split()
    x.append(int(splits[0]))
    y.append(int(splits[1]))

plt.bar(x, y, bottom=0)
plt.show()