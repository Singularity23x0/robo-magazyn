import matplotlib.pyplot as plt

data = [40, 37, 45, 45, 48, 42, 47, 50, 50, 34, 37, 37, 39, 53, 56, 49, 55, 48, 64, 47, 56, 47, 47, 42, 42, 41, 44, 44, 44, 42, 42, 42, 41, 41, 54, 42, 57, 57, 43, 31, 51, 44, 54, 42, 54, 31, 46, 60, 45, 49]

plt.plot(data, c="#4E6E58")
plt.xlabel("generation")
plt.ylabel("minimum solution length in the population")
plt.show()
