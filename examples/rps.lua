-- Simple rock paper scissors cellular automata with only 3 states, besides none

num_cell_types = 4
cell_types = {{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}

NONE = 0
ROCK = 1
PAPER = 2
SCISSORS = 3

-- This can produce some very interesting shapes, 1 makes it a spiraling square, 2 makes it a spiraling rotated square, and 3 makes it a spiraling octagon
threshold = 3

-- Adding randomness makes the spirals more interesting
use_randomness = true

math.randomseed(os.time())

function update(c, n, w, e, s, nw, ne, sw, se)
    neighbors = {n, w, e, s, nw, ne, sw, se}

    num_rock = 0
    num_paper = 0
    num_scissors = 0

    random_term = 0

    if(use_randomness) then
        random_term = math.random(0, 2)
    end

    for i=1, 8, 1 do
        neighbor = neighbors[i]

        if(neighbor == ROCK) then
            num_rock = num_rock + 1
        elseif(neighbor == PAPER) then
            num_paper = num_paper + 1
        elseif(neighbor == SCISSORS) then
            num_scissors = num_scissors + 1
        end
    end

    if(c == ROCK) then
        if(num_paper >= threshold + random_term) then
            return PAPER
        end
    elseif(c == PAPER) then
        if(num_scissors >= threshold + random_term) then
            return SCISSORS
        end
    elseif(c == SCISSORS) then
        if(num_rock >= threshold + random_term) then
            return ROCK
        end
    end

    return c
end