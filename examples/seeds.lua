num_cell_types = 2
cell_types = {{0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}}

OFF = 0
ON = 1

function update(c, n, w, e, s, nw, ne, sw, se)
    neighbors = {n, w, e, s, nw, ne, sw, se}

    if(c == OFF) then
        num = 0
        for i=1, 8, 1 do
            if(neighbors[i] == ON) then
                num = num + 1
            end
        end 

        if(num == 2) then
            return ON
        else
            return OFF
        end
    end

    return OFF
end