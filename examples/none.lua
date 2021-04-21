num_cell_types = 2
cell_types = {{1.0, 1.0, 1.0}, {0.0, 0.0, 0.0}}

options = {neighborhood="morgolus"}
parameters = {generation=true}

function init(width, height)
    print("width:"..width.." height:"..height)
end

function update(nw, ne, sw, se, gen)
    if nw == 1 or ne == 1 or sw == 1 or se == 1 then
        print(nw..", "..ne..", "..sw..", "..se.." : "..(gen % 2))
    end

    return {nw, ne, sw, se}
end
