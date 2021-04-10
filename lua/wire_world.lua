num_cell_types = 4
cell_type_0 = {0.0, 0.0, 0.0} -- Blank Cell / BLACK
cell_type_1 = {1.0, 1.0, 0.0} -- Conductor Cell / YELLOW
cell_type_2 = {0.0, 0.0, 1.0} -- Electron Head Cell / BLUE
cell_type_3 = {1.0, 0.0, 0.0} -- Electron Tail Cell / RED

BLACK = 0
YELLOW = 1
BLUE = 2
RED = 3


function update_cell(nw, nn, ne, ww, cc, ee, sw, ss, se)
    local neighbors = {nw, nn, ne, ww, ee, sw, ss, se}
    
    if(cc == BLACK) then
        -- Empty stays empty
        return BLACK
    
    elseif(cc == BLUE) then
        -- Electron head becomes electron tail
        return RED

    elseif(cc == RED) then
        -- Electron tail becomes a conductor
        return YELLOW

    elseif(cc == YELLOW) then
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