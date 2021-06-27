num_cell_types = 3
cell_types = {{1.0, 1.0, 1.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 1.0}}

options = {neighborhood="von_neumann"}
parameters = {position=true}

-- Any Rule
rule = 90

next_state = {}
next_state["000"] = math.max(math.min(rule & 1, 1), 0)
next_state["001"] = math.max(math.min(rule & 2, 1), 0)
next_state["010"] = math.max(math.min(rule & 4, 1), 0)
next_state["011"] = math.max(math.min(rule & 8, 1), 0)
next_state["100"] = math.max(math.min(rule & 16, 1), 0)
next_state["101"] = math.max(math.min(rule & 32, 1), 0)
next_state["110"] = math.max(math.min(rule & 64, 1), 0)
next_state["111"] = math.max(math.min(rule & 128, 1), 0)

function update(c, n, w, e, s, x, y)
    if(c == 2) then
        if(s == 0) then
            return 0
        elseif(s == 1 or s == 2) then
            return 2
        end
    elseif(c == 1 or (c == 0 and y == 0)) then
        return next_state[w..c..e]
    elseif(c == 0) then
        if(s == 1 or s == 2) then
            return 2
        else
            return 0
        end
    end
end