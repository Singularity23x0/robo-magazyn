let result, board, orders, ordersCopy, solution
let ROW_COUNT, COL_COUNT, ROBOT_COUNT, SCALE
let frameLabel, frame = -1
let ordersLabel, pauseButton;

const BOARD_COLOR = "#355070"
const DARK_BOARD_COLOR = "#3E4756"
const BG_COLOR = "#6881A4"
const colors = ["#f87575", "#b9e6ff", "#5c95ff", "#7e6c6c", "eab464"]

function setup() {
    fetch("../data/out/base-case.json")
        .then(response => {
            return response.json()
        })
        .then(result => {
            orders = result["orders"]
            ordersCopy = JSON.parse(JSON.stringify(orders))
            board = result["board"]
            solution = result["solution"]

            solution = solution[0].map((_, colIndex) => solution.map(row => row[colIndex]))

            ROBOT_COUNT = solution[0].length
            ROW_COUNT = board.length
            COL_COUNT = board[0].length

            createCanvas(.8 * windowHeight * COL_COUNT / ROW_COUNT, .8 * windowHeight)
            SCALE = height / ROW_COUNT - 5
            frameRate(10)
            frameLabel = createP(0)

            ordersLabel = createDiv()
            ordersLabel.addClass('orders')

            pauseButton = createButton("pause")
            pauseButton.mousePressed(() => {
                if (isLooping()) {
                    pauseButton.html("resume")
                    noLoop()
                } else {
                    pauseButton.html("pause")
                    loop()
                }
            })

            frame = 0

            loop()
        }
        );
}

function ordersToHtml() {
    let html = ""
    for (let i = 0; i < ROBOT_COUNT; i++) {
        html += `<p style="color: ${colors[i]}">[${ordersCopy[i].join(', ')}]<p>`
    }
    return html
}

function draw() {

    if (frame < 0) {
        noLoop()
        return
    }

    if (frame == solution.length) {
        // frame = 0;
        // orders_copy = JSON.parse(JSON.stringify(orders));
        noLoop()
        return
    }

    frameLabel.html(`step: ${frame}`)
    ordersLabel.html(ordersToHtml())

    const robot_positions = solution[frame];

    for (let i = 0; i < ROW_COUNT; i++) {
        for (let j = 0; j < COL_COUNT; j++) {
            fill(board[i][j] != -1 ? BOARD_COLOR : DARK_BOARD_COLOR)
            strokeWeight(3)
            stroke(BG_COLOR)
            rect(j * SCALE, i * SCALE, SCALE, SCALE)

            textSize(SCALE * .5)
            textAlign(CENTER, CENTER)
            fill(BG_COLOR + "50")
            noStroke()
            text(board[i][j], (j + .5) * SCALE, (i + .5) * SCALE)

            noStroke()

            // draw robots
            for (let robot_i = 0; robot_i < ROBOT_COUNT; robot_i++) {
                let robot_position = robot_positions[robot_i]
                if (robot_position["row"] == i && robot_position["col"] == j) {
                    fill(colors[robot_i])
                    circle((j + .5) * SCALE, robot_i + (i + .5) * SCALE, .8 * SCALE)
                }
            }

            // draw products
            let rects_count = 0
            let rect_size = SCALE / ROBOT_COUNT - 2

            for (let robot_i = 0; robot_i < ROBOT_COUNT; robot_i++) {

                if (ordersCopy[robot_i].indexOf(board[i][j]) != -1) {
                    fill(colors[robot_i])
                    rect(ROBOT_COUNT + j * SCALE + rects_count * rect_size, -ROBOT_COUNT + (i + 1) * SCALE - rect_size, rect_size, rect_size, 5)
                    rects_count++
                }
            }
        }

    }

    takeProducts(robot_positions)
    frame++;
}

function takeProducts(robot_positions) {
    for (let robot_i = 0; robot_i < ROBOT_COUNT; robot_i++) {
        const action = robot_positions[robot_i]["action"]
        const row = robot_positions[robot_i]["row"]
        const col = robot_positions[robot_i]["col"]
        if (action === "TAKE") {
            let product_index = ordersCopy[robot_i].indexOf(board[row][col])
            if (product_index != -1) {
                ordersCopy[robot_i].splice(product_index, 1)
            }
        }
    }
}