import matplotlib.pyplot as plt
import json

with open("data/out/test-case-p20.json", "r") as f:
    data = json.loads(f.read())

bests = data["best-in-generation"]
all_time_bests = [min(bests[:i+1]) for i in range(len(bests))]

worsts = data["worst-in-generation"]
medians = data["median-in-generation"]

print("best:", min(bests), "worst:", max(worsts))

plt.plot(bests, c="#7D82B8", lw=3, label="best")
plt.plot(worsts, lw=3, c="#EF798A", label="worst")
plt.plot(medians, lw=3, c="#F7A9A8", label="median")
plt.plot(all_time_bests, c="#613F75", lw=3, label="all time best")

plt.xlabel("generation")
plt.legend()
plt.show()
