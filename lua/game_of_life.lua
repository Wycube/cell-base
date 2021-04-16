-- Cells are defined by their color
num_cell_types = 2
-- 1: On / White | 2: Off / Black
cell_types = {{1.0, 1.0, 1.0}, {0.0, 0.0, 0.0}}

options = {edge_case="wrap"}

WHITE = 0
BLACK = 1

function update(c, n, e, w, s, nw, ne, sw, se)
    neighbors = {n, e, w, s, nw, ne, sw, se}
    num = 0

    for i=1, 8, 1 do
        if(neighbors[i] == BLACK) then
            num = num + 1
        end
    end

    if(c == BLACK) then
        if(num == 2 or num == 3) then
            return BLACK
        else
            return WHITE
        end
    elseif(c == WHITE) then
        if(num == 3) then
            return BLACK
        else
            return WHITE
        end
    else
        return WHITE
    end
end