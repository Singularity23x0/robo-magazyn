from random import randint
import json

params = {
    "magazineWidth": 40,
    "magazineHeight": 30,
    "populationCount": 10,
    "robotCount": 4,
    "generationLimit": 50,
    "mutationsFromSolution": 5,
    "robotPositions": [
        {"row": 0, "col": 0},
        {"row": 14, "col": 24},
        {"row": 25, "col": 3},
        {"row": 10, "col": 10}
    ],

    "orders": [
        [1, 3, 5, 7],
        [1, 4, 6],
        [2, 3, 5, 7],
        [1, 2, 4]
    ],
}

width = params["magazineWidth"]
height = params["magazineHeight"]

params["robotEndPositions"] = [
    {"row": 0, "col": 0},
    {"row": height - 1, "col": 0},
    {"row": 0, "col": width - 1},
    {"row": height - 1, "col": width - 1}
]

max_order_num = 7

magazine = [
    [0 for _ in range(width)] for _ in range(height)
]

for pos in params["robotEndPositions"]:
    magazine[pos["row"]][pos["col"]] = -1



for order_num in range(1, max_order_num + 1):
    i = 0

    while i < 30:
        row = randint(0, height - 1)
        col = randint(0, width - 1)

        if magazine[row][col] == 0:
            magazine[row][col] = order_num
            i += 1

params["magazine"] = magazine


with open("data/in/test-case.json", 'w') as f:
    json.dump(params, f)
        


