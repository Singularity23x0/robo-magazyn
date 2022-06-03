import matplotlib.pyplot as plt
import json

with open("data/out/base-case.json", "r") as f:
    data = json.loads(f.read())

bests = data["best-in-generation"]
all_time_bests = [min(bests[:i+1]) for i in range(len(bests))]

plt.plot(bests, c="#7D82B8", lw=3, label="best")
plt.plot(data["worst-in-generation"], lw=3, c="#EF798A", label="worst")
plt.plot(data["median-in-generation"], lw=3, c="#F7A9A8", label="median")
plt.plot(all_time_bests, c="#613F75", lw=3, label="all time best")

plt.xlabel("generation")
plt.legend()
plt.show()
