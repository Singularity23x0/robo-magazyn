const solution = [
    [[1, 1], [7, 2], [2, 1]],
    [[1, 2], [6, 2], [3, 1]],
    [[2, 2], [5, 2], [3, 2]],
]

const board = []
const orders = []

const ROW_COUNT = 10, COL_COUNT = 10
let SCALE;


function setup() {
    let canvas = createCanvas(600, 600 * ROW_COUNT / COL_COUNT)
    SCALE = width / ROW_COUNT
    frameRate(1)
}

let frame = 0
const colors = ["#49BEAA", "#ef767a", "#456990", "#EEB868"]


function draw() {

    background(colors[0])

    let robot_positions = solution[frame];
    frame++;
    if (frame == solution.length) frame = 0;

    for (let i = 0; i < COL_COUNT; i++) {
        for (let j = 0; j < ROW_COUNT; j++) {
            noFill()
            strokeWeight(5)
            rect(i * SCALE, j * SCALE, SCALE, SCALE)

            for (let robot_i = 0; robot_i < robot_positions.length; robot_i++) {
                let robot_position = robot_positions[robot_i]
                if (robot_position[0] == i && robot_position[1] == j) {
                    fill(colors[robot_i + 1])
                    ellipseMode(CORNER)
                    circle(i * SCALE, j * SCALE, SCALE)
                }
            }
        }
    }
}