num_cell_types = 3
cell_types = {{1.0, 1.0, 1.0}, {1.0, 1.0, 0.0}, {0.5, 0.5, 0.5}}

parameters = {position=true}

BLANK = 0
SAND = 1
WALL = 2

sand_commits = {}
commit_size = 0

function update(c, n, w, e, s, nw, ne, sw, se, x, y)
    --print("Array size: "..commit_size)
    
    -- for i=1, commit_size do
    --     if(sand_commits[i] ~= nil) then
    --         print(i.." ("..sand_commits[i]["x"]..", "..sand_commits[i]["y"]..")")
    --     else
    --         print(i.." (nil)")
    --     end
    -- end
    
    if(c == BLANK) then
        -- If there is sand directly above then it becomes sand
        -- If not, then if there is sand to the top left then it becomes sand
        -- Same for top right, if not it stays blank

        -- if(nn == SAND) then
        --     return SAND
        -- elseif(nw == SAND and ww == SAND) then
        --     return SAND
        -- elseif(ne == SAND and ee == SAND) then
        --     return SAND
        -- else
        --     return BLANK
        -- end

        return_sand = false

        if(sand_commits[x..","..y]) then
            sand_commits[x..","..y] = nil
            commit_size = commit_size - 1
            return_sand = true
            --print("Sand moved")
        end

        if(return_sand) then
            return SAND
        else
            return BLANK
        end

    elseif(c == WALL) then
        -- Wall stays wall
        return WALL

    elseif(c == SAND) then
        -- Sand becomes blank if there is blank under it, to the bottom left, or to the bottom right.
        -- Otherwise it stays sand

        if(s == BLANK) then
            --print("("..x..", "..y..") => ("..x..", "..(y-1)..")")
            sand_commits[x..","..(y-1)] = true
            commit_size = commit_size + 1
            --print("Array size: "..commit_size)
            --print("Commit size: "..commit_size)
            return BLANK
        elseif(s == SAND) then
            if(sw == BLANK and w == BLANK) then
                --print("("..x..", "..y..") => ("..(x-1)..", "..(y-1)..")")
                sand_commits[(x-1)..","..(y-1)] = true
                commit_size = commit_size + 1
                --print("Array size: "..commit_size)
                return BLANK
            elseif(se == BLANK and e == BLANK) then
                sand_commits[(x+1)..","..(y-1)] = true
                commit_size = commit_size + 1
                --print("Array size: "..commit_size)
                return BLANK
            else
                return SAND
            end
        else
            return SAND
        end

    end

    return BLANK
end