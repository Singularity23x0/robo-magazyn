from random import randint
import json

params = {
    "magazineWidth": 20,
    "magazineHeight": 15,
    "populationCount": 10,
    "robotCount": 4,
    "generationLimit": 100,
    "mutationsFromSolution": 10,

    "orders": [
        [1, 3, 5, 6, 7, 8, 9],
        [1, 4, 6, 7],
        [2, 3, 5, 7, 8],
        [1, 2, 4, 9]
    ],
}

width = params["magazineWidth"]
height = params["magazineHeight"]

params["robotPositions"] = [
    {"row": 0, "col": 0},
    {"row": height - 1, "col": 0},
    {"row": 0, "col": width - 1},
    {"row": height - 1, "col": width - 1}
]

params["robotEndPositions"] = [
    {"row": 0, "col": 0},
    {"row": height - 1, "col": 0},
    {"row": 0, "col": width - 1},
    {"row": height - 1, "col": width - 1}
]

max_order_num = 9

magazine = [
    [0 for _ in range(width)] for _ in range(height)
]

for pos in params["robotEndPositions"]:
    magazine[pos["row"]][pos["col"]] = -1


for order_num in range(1, max_order_num + 1):
    i = 0

    while i < 20:
        row = randint(0, height - 1)
        col = randint(0, width - 1)

        if magazine[row][col] == 0:
            magazine[row][col] = order_num
            i += 1

params["magazine"] = magazine


with open("data/in/test-case-2.json", 'w') as f:
    json.dump(params, f)
        


