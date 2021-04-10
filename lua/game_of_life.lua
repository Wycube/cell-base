-- Cells are defined by their color
num_cell_types = 2
cell_type_0 = {1.0, 1.0, 1.0} -- On / White
cell_type_1 = {0.0, 0.0, 0.0} -- Off / Black

WHITE = 0
BLACK = 1

function update_cell(nw, nn, ne, ww, cc, ee, sw, ss, se)
    neighbors = {nw, nn, ne, ww, ee, sw, ss, se}
    num = 0

    for i=1, 8, 1 do
        if(neighbors[i] == BLACK) then
            num = num + 1
        end
    end

    if(cc == BLACK) then
        if(num == 2 or num == 3) then
            return BLACK
        else
            return WHITE
        end
    elseif(cc == WHITE) then
        if(num == 3) then
            return BLACK
        else
            return WHITE
        end
    else
        return WHITE
    end
end