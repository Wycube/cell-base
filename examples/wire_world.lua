num_cell_types = 4
cell_types = {{0.0, 0.0, 0.0}, {1.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0}}
-- Blank Cell / BLACK | Conductor Cell / YELLOW | Electron Head Cell / BLUE | Electron Tail Cell / RED

options = {edge_case="wrap"}

BLACK = 0
YELLOW = 1
BLUE = 2
RED = 3


function update(c, n, w, e, s, nw, ne, sw, se)
    local neighbors = {n, w, e, s, nw, ne, sw, se}
    
    if(c == BLACK) then
        -- Empty stays empty
        return BLACK
    
    elseif(c == BLUE) then
        -- Electron head becomes electron tail
        return RED

    elseif(c == RED) then
        -- Electron tail becomes a conductor
        return YELLOW

    elseif(c == YELLOW) then
        -- Become electron head if one or two electron heads are neighboring it
        -- otherwise stay a conductor
        local num = 0

        for i=1, 8, 1 do
            if(neighbors[i] == BLUE) then
                num = num + 1
            end
        end

        if(num == 1 or num == 2) then
            return BLUE
        else
            return YELLOW
        end
    end

    return BLACK
end