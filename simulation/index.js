let result, board, orders, orders_copy, solution
let ROW_COUNT, COL_COUNT, ROBOT_COUNT, SCALE
let frameLabel, frame = -1;

const BOARD_COLOR = "#355070"
const BG_COLOR = "#6881A4"
const colors = ["#44A0C1", "#47D9B2", "#96608E", "#F9F871"]

function setup() {
    fetch("./result.json")
        .then(response => {
            return response.json();
        })
        .then(result => {
            orders = result["orders"]
            orders_copy = JSON.parse(JSON.stringify(orders));
            board = result["board"]
            solution = result["solution"]

            ROBOT_COUNT = solution[0].length
            ROW_COUNT = board.length
            COL_COUNT = board[0].length

            createCanvas(.8 * windowHeight * COL_COUNT / ROW_COUNT, .8 * windowHeight)
            SCALE = height / ROW_COUNT - 5
            frameRate(1)
            frameLabel = createP(0)
            frame = 0

            loop()
        }
        );
}

function draw() {

    if (frame < 0) {
        noLoop();
        return;
    }

    if (frame == solution.length) {
        frame = 0;
        orders_copy = JSON.parse(JSON.stringify(orders));
    }

    const robot_positions = solution[frame];
    frameLabel.elt.innerHTML = frame

    stroke(BG_COLOR)

    for (let i = 0; i < COL_COUNT; i++) {
        for (let j = 0; j < ROW_COUNT; j++) {
            fill(BOARD_COLOR)
            strokeWeight(3)
            stroke(BG_COLOR)
            rect(i * SCALE, j * SCALE, SCALE, SCALE)

            noStroke()

            // draw robots
            for (let robot_i = 0; robot_i < ROBOT_COUNT; robot_i++) {
                let robot_position = robot_positions[robot_i]
                if (robot_position[0][0] == i && robot_position[0][1] == j) {
                    fill(colors[robot_i])
                    circle((i + .5) * SCALE, (j + .5) * SCALE, .8 * SCALE)
                    
                    // take product
                    if (robot_position[1] === "TAKE") {
                        let product_index = orders_copy[robot_i].indexOf(board[i][j])
                        if (product_index != -1) {
                            orders_copy[robot_i].splice(product_index, 1)
                        }
                    }
                }
            }

            // draw products
            let small_circles_count = 0
            let diameter = SCALE / ROBOT_COUNT - 2

            for (let robot_i = 0; robot_i < ROBOT_COUNT; robot_i++) {

                if (orders_copy[robot_i].indexOf(board[i][j]) != -1) {
                    fill(colors[robot_i])
                    circle(ROBOT_COUNT + i * SCALE + (small_circles_count + .5) * (diameter), -ROBOT_COUNT + (j + 1) * SCALE - diameter / 2, diameter)
                    small_circles_count++
                }
            }
        }

    }

    frame++;
}