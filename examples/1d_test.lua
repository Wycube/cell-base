num_cell_types = 2
cell_types = {{1.0, 1.0, 1.0}, {0.0, 0.0, 0.0}}

options = {dimensions=1}

-- Any Rule
rule = 129

next_state = {}
next_state["000"] = math.max(math.min(rule & 1, 1), 0)
next_state["001"] = math.max(math.min(rule & 2, 1), 0)
next_state["010"] = math.max(math.min(rule & 4, 1), 0)
next_state["011"] = math.max(math.min(rule & 8, 1), 0)
next_state["100"] = math.max(math.min(rule & 16, 1), 0)
next_state["101"] = math.max(math.min(rule & 32, 1), 0)
next_state["110"] = math.max(math.min(rule & 64, 1), 0)
next_state["111"] = math.max(math.min(rule & 128, 1), 0)

function update(c, l, r)
    return next_state[l..c..r]
end