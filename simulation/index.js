const board = Array(10).fill().map(_ => Array(8))

board[6][2] = 5
board[7][2] = 1
board[3][2] = 3

const ROW_COUNT = board.length
const COL_COUNT = board[0].length

const orders = [[1, 3, 5], [2, 5], [1, 5]]
let orders_copy = JSON.parse(JSON.stringify(orders));

const solution = [
    [[1, 1], [7, 2], [2, 1]],
    [[1, 2], [6, 2], [3, 1]],
    [[2, 2], [5, 2], [3, 2]],
]

const ROBOT_COUNT = solution[0].length

let SCALE;

function setup() {
    createCanvas(.8 * windowHeight * COL_COUNT / ROW_COUNT, .8 * windowHeight)
    SCALE = height / ROW_COUNT - 5
    frameRate(1)
}

let frame = 0

const BG_COLOR = "#355070"
const STROKE_COLOR = "#6881A4"

const colors = ["#44A0C1", "#47D9B2", "#96608E", "#F9F871"]

function draw() {

    if (frame == solution.length) {
        frame = 0;
        orders_copy = JSON.parse(JSON.stringify(orders));
    } 

    const robot_positions = solution[frame];

    stroke(STROKE_COLOR)

    for (let i = 0; i < COL_COUNT; i++) {
        for (let j = 0; j < ROW_COUNT; j++) {
            fill(BG_COLOR)
            strokeWeight(3)
            stroke(STROKE_COLOR)
            rect(i * SCALE, j * SCALE, SCALE, SCALE)

            noStroke()
            // draw robots
            for (let robot_i = 0; robot_i < ROBOT_COUNT; robot_i++) {
                let robot_position = robot_positions[robot_i]
                if (robot_position[0] == i && robot_position[1] == j) {
                    fill(colors[robot_i])
                    circle((i + .5) * SCALE, (j + .5) * SCALE, .8 * SCALE)

                    let product_index = orders_copy[robot_i].indexOf(board[i][j])
                    if (product_index != -1) {
                        orders_copy[robot_i].splice(product_index, 1)
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