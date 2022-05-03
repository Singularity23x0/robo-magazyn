const board = Array(10).fill().map(_ => Array(8))

board[6][2] = 5
board[7][2] = 1
board[3][2] = 3

const ROW_COUNT = board.length
const COL_COUNT = board[0].length

const orders = [[1, 3, 5], [2, 5], [1, 5]]

const solution = [
    [[1, 1], [7, 2], [2, 1]],
    [[1, 2], [6, 2], [3, 1]],
    [[2, 2], [5, 2], [3, 2]],
]

const ROBOT_COUNT = solution[0].length

let SCALE;

function setup() {
    createCanvas(windowHeight * COL_COUNT / ROW_COUNT, windowHeight)
    SCALE = height / ROW_COUNT
    frameRate(1)
}

let frame = 0
const colors = ["#49BEAA", "#ef767a", "#456990", "#EEB868"]

function draw() {

    background(colors[0])

    const robot_positions = solution[frame];
    frame++;
    if (frame == solution.length) frame = 0;

    for (let i = 0; i < COL_COUNT; i++) {
        for (let j = 0; j < ROW_COUNT; j++) {
            noFill()
            strokeWeight(3)
            stroke(255)
            rect(i * SCALE, j * SCALE, SCALE, SCALE)

            // draw robots
            for (let robot_i = 0; robot_i < ROBOT_COUNT; robot_i++) {
                let robot_position = robot_positions[robot_i]
                if (robot_position[0] == i && robot_position[1] == j) {
                    fill(colors[robot_i + 1])
                    ellipseMode(CORNER)
                    strokeWeight(2)
                    circle(i * SCALE, j * SCALE, SCALE)
                }
            }

            // draw products
            let robot_goal_count = 0
            for (let robot_i = 0; robot_i < ROBOT_COUNT; robot_i++) {
                let diameter = SCALE / ROBOT_COUNT

                if (orders[robot_i].indexOf(board[i][j]) != -1) {
                    fill(colors[robot_i + 1])
                    strokeWeight(1)
                    circle(i * SCALE + robot_goal_count * diameter, (j + 1) * SCALE - diameter, diameter)
                    robot_goal_count++
                }
            }
        }
    }
}