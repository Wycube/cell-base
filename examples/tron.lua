num_cell_types = 2
cell_types = {{0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}}

OFF = 0
ON = 1

options = {neighborhood="morgolus"}

function update(nw, ne, sw, se)
    if(nw == ON and ne == ON and sw == ON and se == ON) then
        return {OFF, OFF, OFF, OFF}
    elseif(nw == OFF and ne == OFF and sw == OFF and se == OFF) then
        return {ON, ON, ON, ON}
    else
        return {nw, ne, sw, se}
    end
end