num_cell_types = 3
cell_types = {{1.0, 1.0, 1.0}, {0.0, 0.0, 1.0}, {0.5, 0.5, 0.5}}

BLANK = 0
WATER = 1
WALL = 2

options = {neighborhood="morgolus", edge_case="cell", edge_cell_type=WALL}


-- Possible configurations and new states
new_states = {}
new_states["0001"] = {0, 0, 1, 0}
new_states["0010"] = {0, 0, 0, 1}
new_states["0100"] = {0, 0, 0, 1}
new_states["0101"] = {0, 0, 1, 1}
new_states["0110"] = {0, 0, 1, 1}
new_states["0111"] = {1, 0, 1, 1}
new_states["1000"] = {0, 0, 1, 0}
new_states["1001"] = {0, 0, 1, 1}
new_states["1010"] = {0, 0, 1, 1}
new_states["1011"] = {0, 1, 1, 1}
new_states["1100"] = {0, 0, 1, 1}
new_states["1101"] = {1, 1, 1, 0}
new_states["1110"] = {1, 1, 0, 1}

new_states["1020"] = {0, 0, 2, 1}
new_states["0120"] = {0, 0, 2, 1}
new_states["1200"] = {0, 2, 1, 0}
new_states["1102"] = {0, 1, 1, 2}
new_states["1120"] = {1, 0, 2, 1}
new_states["1002"] = {0, 0, 1, 2}
new_states["1202"] = {0, 2, 1, 2}
new_states["2120"] = {2, 0, 2, 1}
new_states["2100"] = {2, 0, 0, 1}
new_states["1200"] = {0, 2, 1, 0}
new_states["1210"] = {0, 2, 1, 1}


function update(nw, ne, sw, se, gen)
    next_state = new_states[nw..ne..sw..se]

    if next_state == nil then
        return {nw, ne, sw, se}
    end

    return next_state
end