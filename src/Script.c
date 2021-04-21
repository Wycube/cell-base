#include "Script.h"

#include <lualib.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>

#include "Utility.h"

typedef int bool;

//TODO: 
// - Do something better than exit on error
// - Add 1 dimensional cellular automata

// Utility Functions

static void stack_dump(lua_State *L);

bool get_bool(lua_State *L, const char *name, bool default_value) {
    //Retrieve the global
    lua_getglobal(L, name);
    
    //Check whether it exists or is a bool
    if(!lua_isboolean(L, -1)) {
        printf("[INFO]: No boolean variable '%s' exists using value %s instead.\n", name, default_value ? "true" : "false");
        return default_value;
    } else {
        bool val = lua_toboolean(L, -1);
        lua_pop(L, 1);

        return val;
    }
}

int get_int(lua_State *L, const char *name, int default_value) {
    //Retrieve the global
    lua_getglobal(L, name);
    
    //Check whether it exists or is a bool
    if(!lua_isinteger(L, -1)) {
        printf("[INFO]: No integer variable '%s' exists using value %i instead.\n", name, default_value);
        return default_value;
    } else {
        int val = lua_tointeger(L, -1);
        lua_pop(L, 1);

        return val;
    }
}

const char* get_string(lua_State *L, const char *name, const char *default_value) {
    //Retrieve the global
    lua_getglobal(L, name);
    
    //Check whether it exists or is a bool
    if(!lua_isstring(L, -1)) {
        printf("[INFO]: No string variable '%s' exists using value '%s' instead.\n", name, default_value);
        return default_value;
    } else {
        const char *val = lua_tostring(L, -1);
        lua_pop(L, 1);

        return val;
    }
}

bool get_table_bool(lua_State *L, const char *table_name, const char *field_name, bool default_value) {
    //Retrieve table
    lua_getglobal(L, table_name);

    //Validate table
    if(!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return default_value;
    }

    //Push field name and get item from table
    lua_pushstring(L, field_name);
    lua_gettable(L, -2);

    //Validate item
    if(!lua_isboolean(L, -1)) {
        printf("[INFO]: No boolean field '%s' exists in table '%s', using value %s instead.\n", field_name, table_name, default_value ? "true" : "false");
        lua_pop(L, 2);
        return default_value;
    }

    bool val = lua_toboolean(L, -1);
    lua_pop(L, 2);

    return val;
}

int get_table_int(lua_State *L, const char *table_name, const char *field_name, int default_value) {
    //Retrieve table
    lua_getglobal(L, table_name);

    //Validate table
    if(!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return default_value;
    }

    //Push field name and get item from table
    lua_pushstring(L, field_name);
    lua_gettable(L, -2);

    //Validate item
    if(!lua_isinteger(L, -1)) {
        printf("[INFO]: No integer field '%s' exists in table '%s', using value %i instead.\n", field_name, table_name, default_value);
        lua_pop(L, 2);
        return default_value;
    }

    int val = lua_tointeger(L, -1);
    lua_pop(L, 2);

    return val;
}

const char* get_table_string(lua_State *L, const char *table_name, const char *field_name, const char *default_value) {
    //Retrieve table
    lua_getglobal(L, table_name);

    //Validate table
    if(!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return default_value;
    }

    //Push field name and get item from table
    lua_pushstring(L, field_name);
    lua_gettable(L, -2);

    //Validate item
    if(!lua_isstring(L, -1)) {
        printf("[INFO]: No string field '%s' exists in table '%s', using value '%s' instead.\n", field_name, table_name, default_value);
        lua_pop(L, 2);
        return default_value;
    }

    const char *val = lua_tostring(L, -1);
    lua_pop(L, 2);

    return val;
}

uint32_t get_cell_color_at(lua_State *L, int index) {
    //Get cell_types table
    lua_getglobal(L, "cell_types");

    //Validate table, exit on error
    if(!lua_istable(L, -1)) {
        printf("[ERROR]: Table 'cell_types' not found, aborting!\n");
        exit(-1);
    }

    //Get table in the table
    lua_pushinteger(L, index);
    lua_gettable(L, -2);

    //Validate table, exit on error
    if(!lua_istable(L, -1)) {
        printf("[ERROR]: No table found at index %i of table 'cell_types', abortin!\n", index);
        exit(-1);
    }

    //Get components from table
    float r, g, b;
    
    lua_pushinteger(L, 1);
    lua_gettable(L, -2);
    r = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_pushinteger(L, 2);
    lua_gettable(L, -2);
    g = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_pushinteger(L, 3);
    lua_gettable(L, -2);
    b = lua_tonumber(L, -1);
    lua_pop(L, 3);

    return frgb_to_int(r, g, b);
}

int str_to_neighborhood(const char *name) {
    if(strcmp(name, "moore") == 0) {
        return MOORE;
    } else if(strcmp(name, "von_neumann") == 0) {
        return VON_NEUMANN;
    } else if(strcmp(name, "von_neumann_ext") == 0) {
        return VON_NEUMANN_EXT;
    } else if(strcmp(name, "moore_von_neumann") == 0) {
        return MOORE_VON_NEUMANN;
    } else if(strcmp(name, "morgolus") == 0) {
        return MORGOLUS;
    } else if(strcmp(name, "none") == 0) {
        return NONE;
    } else {
        return -1;
    }
}

static void stack_dump(lua_State *L) {
    int i;
    int top = lua_gettop(L);

    printf("Size: %i | ", top);

    for (i = 1; i <= top; i++) {  /* repeat for each level */
        int t = lua_type(L, i);
        switch (t) {

            case LUA_TSTRING:  /* strings */
            printf("`%s'", lua_tostring(L, i));
            break;

            case LUA_TBOOLEAN:  /* booleans */
            printf(lua_toboolean(L, i) ? "true" : "false");
            break;

            case LUA_TNUMBER:  /* numbers */
            printf("%g", lua_tonumber(L, i));
            break;

            default:  /* other values */
            printf("%s", lua_typename(L, t));
            break;

        }
        printf("  ");  /* put a separator */
    }
    printf("\n");  /* end the listing */
}

// Script Functions

Script get_script(const char *file_path) {
    lua_State *state = luaL_newstate();
    luaL_openlibs(state);

    //Run file in order to get function and variables definitions
    //If it fails get error, either doesn't exist or there's a syntax error
    if(luaL_dofile(state, file_path) != 0) {
        printf("[ERROR]: %s, aborting!\n", lua_tostring(state, -1));
        exit(-1);
    }

    //Get number of cell types
    int num_cell_types = get_int(state, "num_cell_types", 0);

    if(num_cell_types == 0) {
        printf("[ERROR]: No num_cell_types provided, aborting!");
        exit(-1);
    }

    //Get the cell types
    uint32_t *cell_types = (uint32_t*)malloc(sizeof(uint32_t) * num_cell_types);

    int i;
    for(i = 0; i < num_cell_types; i++) {
        //Add one to i because lua has 1-indexed arrays
        cell_types[i] = get_cell_color_at(state, i + 1);
    }

    //Get generic options
    //Buffer field (bool)
    bool buffer_field = get_table_bool(state, "options", "buffer_field", true);

    //Edge case (string)
    const char *edge_case_str = get_table_string(state, "options", "edge_case", "cell");

    if(!(strcmp(edge_case_str, "wrap") == 0 || strcmp(edge_case_str, "cell") == 0)) {
        printf("[INFO]: Invalid edge_case value '%s', using 'cell' instead.\n", edge_case_str);
        edge_case_str = "cell";
    }

    int edge_case = strcmp(edge_case_str, "wrap") == 0 ? WRAP : CELL;

    //Edge Case Cell type
    int edge_cell_type = get_table_int(state, "options", "edge_cell_type", 0);

    if(edge_cell_type >= num_cell_types) {
        printf("[INFO]: Invalid edge_cell_type value %i, using 0 instead.\n", edge_cell_type);
        edge_cell_type = 0;
    }

    //Field Width (int) and Field Height (int)
    int field_width = get_table_int(state, "options", "field_width", 50);
    int field_height = get_table_int(state, "options", "field_height", 50);

    if(field_width <= 0 || field_height <= 0) {
        printf("[ERROR]: Invalid field width or height value (w=%i, h=%i), aborting!", field_width, field_height);
        exit(-1);
    }

    //Get dimensions and validate
    int dimensions = get_table_int(state, "options", "dimensions", 2);

    if(dimensions != 1 && dimensions != 2) {
        printf("[INFO]: Invalid dimensions value %i, using 2 instead.\n", dimensions);
        dimensions = 2;
    }

    //Get neighborhood
    const char *neighborhood_str = get_table_string(state, "options", "neighborhood", "moore");

    if(!(strcmp(neighborhood_str, "moore") == 0 || strcmp(neighborhood_str, "von_neumann") == 0 || strcmp(neighborhood_str, "von_neumann_ext") == 0 || 
    strcmp(neighborhood_str, "moore_von_neumann") == 0 || strcmp(neighborhood_str, "morgolus") == 0)) {
        printf("[INFO]: Invalid neighborhood value '%s', using 'moore' instead.\n", neighborhood_str);
        neighborhood_str = "moore";
    }

    int neighborhood = str_to_neighborhood(neighborhood_str);

    //Type
    int type = neighborhood == MORGOLUS ? BLOCK : REGULAR;

    //Get parameter options
    bool position = get_table_bool(state, "parameters", "position", false);
    bool generation = get_table_bool(state, "parameters", "generation", false);

    //Run possible init function
    lua_getglobal(state, "init");
    if(lua_isfunction(state, -1)) {
        //Push width and height
        lua_pushinteger(state, field_width);
        lua_pushinteger(state, field_height);

        if(lua_pcall(state, 2, 0, 0) != 0) {
            printf("Function '%s' execution failed:\nError: %s!\nStackdump: ", "init", lua_tostring(state, -1));
            stack_dump(state);
            exit(-1);
        }
    }

    //Create script struct
    Script s = {file_path, state, 0, type, num_cell_types, cell_types, buffer_field, edge_case, edge_cell_type, field_width, field_height, dimensions, neighborhood, position, generation};

    return s;
}

void free_script(Script *script) {
    if(script->cell_types != NULL) {
        free(script->cell_types);
    }
}

void push_cell(Script *script, Playfield *playfield, int x, int y) {
    if(x >= 0 && y >= 0 && x < (int)playfield->width && y < (int)playfield->height) {
        lua_pushinteger(script->L, playfield->cell_field[x + y * playfield->width]);
    } else {
        if(script->opt_edge_case == WRAP) {
            //Wrap negatives here
            x = x < 0 ? 50 + x : x;
            y = y < 0 ? 50 + y : y;
            lua_pushinteger(script->L, playfield->cell_field[(x % playfield->width) + (y % playfield->height) * playfield->width]);
        } else {
            lua_pushinteger(script->L, script->opt_edge_cell_type);
        }
    }
}

void one_d_cell_update(Script *script, Playfield *playfield);
void regular_cell_update(Script *script, Playfield *playfield);
void block_cell_update(Script *script, Playfield *playfield);

void run_cell_update(Script *script, Playfield *playfield) {
    if(script->opt_dimensions == 1) {
        one_d_cell_update(script, playfield);
    } else {
        if(script->opt_neighborhood == MORGOLUS) {
            block_cell_update(script, playfield);
        } else {
            regular_cell_update(script, playfield);
        }
    }
}

// The function parameter's structure looks like this
// They are optional if they have a ?
// update(c, <l, r>?, <x>?, <generation>?)
void one_d_cell_update(Script *script, Playfield *playfield) {
    int *line_buffer;

    if(script->opt_buffer_field) {
        line_buffer = (int*)malloc(sizeof(int) * playfield->width);
        memcpy(line_buffer, playfield->cell_field, sizeof(int) * playfield->width);
    }

    //Copy the last generations up
    for(int y = (int)playfield->width - 2; y >= 0; y--) {
        for(int x = 0; x < (int)playfield->width; x++) {
            int curr_cell = playfield->cell_field[x + y * playfield->width];

            playfield->cell_field[x + (y + 1) * playfield->width] = curr_cell;
            playfield->field[x + (y + 1) * playfield->width] = script->cell_types[curr_cell];
        }
    }

    int col_index;

    for(int x = 0; x < (int)playfield->width; x++) {
        //Push function onto the stack
        lua_getglobal(script->L, "update");

        if(!lua_isfunction(script->L, -1)) {
            printf("Error: No function '%s'!\n", "update");
            exit(-1);
        }

        int num_params = 0;

        //Push parameters onto the stack
        //center
        push_cell(script, playfield, (x + 0), 0);
        num_params += 1;

        if(script->opt_neighborhood != NONE) {
            //Push parameters l, r
            push_cell(script, playfield, (x - 1), 0);
            push_cell(script, playfield, (x + 1), 0);
            num_params += 2;
        }

        if(script->param_position) {
            lua_pushinteger(script->L, x);
            num_params += 1;
        }
        if(script->param_generation) {
            lua_pushinteger(script->L, script->generation);
            num_params += 1;
        }

        if(lua_pcall(script->L, num_params, 1, 0) != 0) {
            printf("Function '%s' execution failed:\nError: %s!\nStackdump: ", "update", lua_tostring(script->L, -1));
            stack_dump(script->L);
            exit(-1);
        }

        //Get the cell type that was returned
        col_index = lua_tointeger(script->L, -1);
        lua_pop(script->L, 1);
        
        if(script->opt_buffer_field) {
            line_buffer[x + 0 * playfield->width] = col_index;
        } else {
            playfield->cell_field[x + 0 * playfield->width] = col_index;
        }
        playfield->field[x + 0 * playfield->width] = script->cell_types[col_index];
    }

    if(script->opt_buffer_field) {
        memcpy(playfield->cell_field, line_buffer, sizeof(int) * playfield->width);
        free(line_buffer);
    }
}

// The function parameter's structure looks like this
// They are optional if they have a ?
// update(c, <n, w, e, s>?, <nw, ne, sw, se>?, <nn, ww, ee, ss>?, <x, y>?, <generation>?)
void regular_cell_update(Script *script, Playfield *playfield) {
    int *buffer;

    if(script->opt_buffer_field) {
        buffer = (int*)malloc(sizeof(int) * playfield->width * playfield->height);
        memcpy(buffer, playfield->cell_field, sizeof(int) * playfield->width * playfield->height);
    }

    int col_index;

    for(int x = 0; x < (int)playfield->width; x++) {
        for(int y = 0; y < (int)playfield->height; y++) {
            //Push function onto the stack
            lua_getglobal(script->L, "update");

            if(!lua_isfunction(script->L, -1)) {
                printf("Error: No function '%s'!\n", "update");
                exit(-1);
            }

            int num_params = 0;

            //Push parameters onto the stack
            //center
            push_cell(script, playfield, (x + 0), (y + 0));
            num_params += 1;

            if(script->opt_neighborhood != NONE) {
                //Push parameters n, w, e, s
                push_cell(script, playfield, (x + 0), (y + 1));
                push_cell(script, playfield, (x - 1), (y + 0));
                push_cell(script, playfield, (x + 1), (y + 0));
                push_cell(script, playfield, (x - 0), (y - 1));
                num_params += 4;
            }

            if(script->opt_neighborhood == MOORE || script->opt_neighborhood == MOORE_VON_NEUMANN) {
                //Push parameters nw, ne, sw, se
                push_cell(script, playfield, (x - 1), (y + 1));
                push_cell(script, playfield, (x + 1), (y + 1));
                push_cell(script, playfield, (x - 1), (y - 1));
                push_cell(script, playfield, (x + 1), (y - 1));
                num_params += 4;
            }
            if(script->opt_neighborhood == VON_NEUMANN_EXT || script->opt_neighborhood == MOORE_VON_NEUMANN) {
                //Push parameters nn, ww, ee, ss
                push_cell(script, playfield, (x + 0), (y + 2));
                push_cell(script, playfield, (x - 2), (y + 0));
                push_cell(script, playfield, (x + 2), (y + 0));
                push_cell(script, playfield, (x + 0), (y - 2));
                num_params += 4;
            }

            if(script->param_position) {
                lua_pushinteger(script->L, x);
                lua_pushinteger(script->L, y);
                num_params += 2;
            }
            if(script->param_generation) {
               lua_pushinteger(script->L, script->generation);
               num_params += 1;
            }

            if(lua_pcall(script->L, num_params, 1, 0) != 0) {
                printf("Function '%s' execution failed:\nError: %s!\nStackdump: ", "update", lua_tostring(script->L, -1));
                stack_dump(script->L);
                exit(-1);
            }

            //Get the cell type that was returned
            col_index = lua_tointeger(script->L, -1);
            lua_pop(script->L, 1);
            
            if(script->opt_buffer_field) {
                buffer[x + y * playfield->width] = col_index;
            } else {
                playfield->cell_field[x + y * playfield->width] = col_index;
            }
            playfield->field[x + y * playfield->width] = script->cell_types[col_index];
        }
    }

    if(script->opt_buffer_field) {
        memcpy(playfield->cell_field, buffer, sizeof(int) * playfield->width * playfield->height);
        free(buffer);
    }
}

// No buffer is needed here because it wouldn't matter anyway as
// the blocks don't overlap and therefore can't change anything outside it.
// update(nw, ne, sw, se, <x, y>?, <generation>?)
// x, y are the top left cell in the block.
void block_cell_update(Script *script, Playfield *playfield) {
    //assert(playfield->width % 2 == 0);
    //assert(playfield->height % 2 == 0);

    int block_offset = script->generation % 2;

    for(int x = -block_offset; x < (int)playfield->width; x += 2) {
        for(int y = -block_offset; y < (int)playfield->height; y += 2) {

            int w = x;
            int e = x + 1;
            int n = y + 1;
            int s = y;

            //Push function onto the stack
            lua_getglobal(script->L, "update");

            if(!lua_isfunction(script->L, -1)) {
                printf("Error: No function '%s'!\n", "update");
                exit(-1);
            }

            int num_params = 0;

            //Push parameters onto the stack
            //nw, ne, sw, se
            push_cell(script, playfield, w, n);
            push_cell(script, playfield, e, n);
            push_cell(script, playfield, w, s);
            push_cell(script, playfield, e, s);
            num_params += 4;

            //Optional parameters
            if(script->param_position) {
                lua_pushinteger(script->L, w);
                lua_pushinteger(script->L, n);
                num_params += 2;
            }
            if(script->param_generation) {
               lua_pushinteger(script->L, script->generation);
               num_params += 1;
            }

            if(lua_pcall(script->L, num_params, 1, 0) != 0) {
                printf("Function '%s' execution failed:\nError: %s!\nStackdump: ", "update", lua_tostring(script->L, -1));
                stack_dump(script->L);
                exit(-1);
            }

            //Get return values. They are a table containing the next states of the block
            //in the form {nw, ne, sw, se}
            lua_pushinteger(script->L, 1);
            lua_gettable(script->L, -2);
            int nw = lua_tointeger(script->L, -1);
            lua_pop(script->L, 1);

            lua_pushinteger(script->L, 2);
            lua_gettable(script->L, -2);
            int ne = lua_tointeger(script->L, -1);
            lua_pop(script->L, 1);

            lua_pushinteger(script->L, 3);
            lua_gettable(script->L, -2);
            int sw = lua_tointeger(script->L, -1);
            lua_pop(script->L, 1);

            lua_pushinteger(script->L, 4);
            lua_gettable(script->L, -2);
            int se = lua_tointeger(script->L, -1);
            lua_pop(script->L, 2);

            if(script->opt_edge_case == WRAP) {
                w = w < 0 ? playfield->width + w : w % playfield->width;
                e = e < 0 ? playfield->width + e : e % playfield->width;
                n = n < 0 ? playfield->height + n : n % playfield->height;
                s = s < 0 ? playfield->height + s : s % playfield->height;
            }

            if(w < (int)playfield->width && w >= 0) {
                //nw
                if(n < (int)playfield->height && n >= 0) {
                    playfield->cell_field[w + n * playfield->width] = nw;
                    playfield->field[w + n * playfield->width] = script->cell_types[nw];
                }
                //sw
                if(s < (int)playfield->height && s >= 0) {
                    playfield->cell_field[w + s * playfield->width] = sw;
                    playfield->field[w + s * playfield->width] = script->cell_types[sw];
                }
            }
            if(e < (int)playfield->width && e >= 0) {
                //ne
                if(n < (int)playfield->height && n >= 0) {
                    playfield->cell_field[e + n * playfield->width] = ne;
                    playfield->field[e + n * playfield->width] = script->cell_types[ne];
                }
                //se
                if(s < (int)playfield->height && s >= 0) {
                    playfield->cell_field[e + s * playfield->width] = se;
                    playfield->field[e + s * playfield->width] = script->cell_types[se];
                }
            }
        }
    }
}
