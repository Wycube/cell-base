num_cell_types = 2
cell_types = {{1.0, 1.0, 1.0}, {0.0, 0.0, 0.0}}

options = {neighborhood="morgolus"}

function update(nw, ne, sw, se)
    if nw == 1 then
        print(nw..", "..ne..", "..sw..", "..se)
    end

    return {nw, ne, sw, se}
end
