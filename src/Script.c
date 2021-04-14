#include "Script.h"

#include <lualib.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>

#include "Utility.h"

typedef int bool;

//TODO: 
// - Do something better than exit on error
// - Add init function
// - Error check color components in get_cell_color_at
// - Add functionality for neighborhood
// - Add functionality for field width and height
// - Add 1 dimensional cellular automata

// Utility Functions

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
        return default_value;
    }

    //Push field name and get item from table
    lua_pushstring(L, field_name);
    lua_gettable(L, -2);

    //Validate item
    if(!lua_isboolean(L, -1)) {
        printf("[INFO]: No boolean field '%s' exists in table '%s', using value %s instead.\n", field_name, table_name, default_value ? "true" : "false");
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
        return default_value;
    }

    //Push field name and get item from table
    lua_pushstring(L, field_name);
    lua_gettable(L, -2);

    //Validate item
    if(!lua_isinteger(L, -1)) {
        printf("[INFO]: No integer field '%s' exists in table '%s', using value %i instead.\n", field_name, table_name, default_value);
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
        return default_value;
    }

    //Push field name and get item from table
    lua_pushstring(L, field_name);
    lua_gettable(L, -2);

    //Validate item
    if(!lua_isstring(L, -1)) {
        printf("[INFO]: No string field '%s' exists in table '%s', using value '%s' instead.\n", field_name, table_name, default_value);
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
    lua_pop(L, 1);

    return frgb_to_int(r, g, b);
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

    //Get dimensions and validate
    int dimensions = get_int(state, "dimensions", 2);

    if(dimensions != 1 && dimensions != 2) {
        printf("[INFO]: Invalid dimensions value %i, using 2 instead.\n", dimensions);
        dimensions = 2;
    }

    //Get neighborhood
    const char *neighborhood = get_string(state, "neighborhood", "moore");

    if(!(strcmp(neighborhood, "moore") == 0 || strcmp(neighborhood, "von_neumann") == 0 || strcmp(neighborhood, "von_neumann_ext") == 0 || 
    strcmp(neighborhood, "moore_von_neumann") == 0 || strcmp(neighborhood, "morgolus") == 0)) {
        printf("[INFO]: Invalid neighborhood value '%s', using 'moore' instead.\n", neighborhood);
        neighborhood = "moore";
    }

    //Get generic options
    //Buffer field (bool)
    bool buffer_field = get_table_bool(state, "options", "buffer_field", true);

    //Edge case (string)
    const char *edge_case = get_table_string(state, "options", "edge_case", "cell_0");

    if(!(strcmp(edge_case, "wrap") == 0 || strcmp(edge_case, "cell_0") == 0)) {
        printf("[INFO]: Invalid edge_case value '%s', using 'cell_0' instead.\n", edge_case);
        edge_case = "cell_0";
    }

    //Field Width (int) and Field Height (int)
    int field_width = get_table_int(state, "options", "field_width", 50);
    int field_height = get_table_int(state, "options", "field_height", 50);

    if(field_width <= 0 || field_height <= 0) {
        printf("[ERROR]: Invalid field width or height value (w=%i, h=%i), aborting!", field_width, field_height);
        exit(-1);
    }

    //Get parameter options
    bool position = get_table_bool(state, "parameters", "position", false);
    bool generation = get_table_bool(state, "parameters", "generation", false);

    //Create script struct
    Script s = {file_path, state, num_cell_types, cell_types, buffer_field, edge_case, field_width, field_height, position, generation, dimensions, neighborhood};

    return s;
}

void free_script(Script *script) {
    if(script->cell_types != NULL) {
        free(script->cell_types);
    }
}

static void stackDump(lua_State *L) {
    int i;
    int top = lua_gettop(L);

    printf("Size: %i\n", top);

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
        printf(" ");  /* put a separator */
    }
    printf("\n");  /* end the listing */
}

void push_cell(lua_State *L, int *cell_field, int x, int y, uint32_t width, uint32_t height, bool wrap) {
    if(x >= 0 && y >= 0 && x < width && y < height) {
        lua_pushinteger(L, cell_field[x + y * width]);
    } else {
        if(wrap) {
            //Wrap negatives here
            x = x < 0 ? 50 + x : x;
            y = y < 0 ? 50 + y : y;
            lua_pushinteger(L, cell_field[(x % width) + (y % height) * width]);
        } else {
            lua_pushinteger(L, 0);
        }
    }
}


// The function parameter's structure looks like this
// They are optional if they have a ?
// update_cell(<neighbors 9>, <position 2>?, <generation 1>?)
void run_cell_update(Script *script, Playfield *playfield) {
    int *buffer;
    bool wrap = strcmp(script->edge_case, "wrap") == 0;

    if(script->buffer_field) {
        buffer = (int*)malloc(sizeof(int) * playfield->width * playfield->height);
        memcpy(buffer, playfield->cell_field, sizeof(int) * playfield->width * playfield->height);
    }

    int col_index;

    for(int x = 0; x < (int)playfield->width; x++) {
        for(int y = 0; y < (int)playfield->height; y++) {
        //for(int y = (int)playfield->height - 1; y >= 0; y--) {
            //Push function onto the stack
            lua_getglobal(script->L, "update_cell");

            if(!lua_isfunction(script->L, -1)) {
                printf("Error: No function '%s'!\n", "update_cell");
                exit(-1);
            }

            //Push parameters onto the stack
            //cc, nw, nn, ne, ww, ee, sw, ss, se
            push_cell(script->L, playfield->cell_field, (x - 0), (y - 0), playfield->width, playfield->height, wrap);
            push_cell(script->L, playfield->cell_field, (x - 1), (y + 1), playfield->width, playfield->height, wrap);
            push_cell(script->L, playfield->cell_field, (x - 0), (y + 1), playfield->width, playfield->height, wrap);
            push_cell(script->L, playfield->cell_field, (x + 1), (y + 1), playfield->width, playfield->height, wrap);
            push_cell(script->L, playfield->cell_field, (x - 1), (y - 0), playfield->width, playfield->height, wrap);
            push_cell(script->L, playfield->cell_field, (x + 1), (y - 0), playfield->width, playfield->height, wrap);
            push_cell(script->L, playfield->cell_field, (x - 1), (y - 1), playfield->width, playfield->height, wrap);
            push_cell(script->L, playfield->cell_field, (x - 0), (y - 1), playfield->width, playfield->height, wrap);
            push_cell(script->L, playfield->cell_field, (x + 1), (y - 1), playfield->width, playfield->height, wrap);

            if(script->position) {
                lua_pushinteger(script->L, x);
                lua_pushinteger(script->L, y);
            }
            //TODO: this
            //if(script->generation) {
            //    lua_pushinteger(script->L, )
            //}
            
            if(lua_pcall(script->L, 9 + script->position * 2 /*+ script->generation*/, 1, 0) != 0) {
                printf("Function '%s' execution failed:\nError: %s!\nStackdump: ", "update_cell", lua_tostring(script->L, -1));
                stackDump(script->L);
                exit(-1);
            }

            //Get the table that was returned
            col_index = lua_tointeger(script->L, -1);
            lua_pop(script->L, 1);
            
            if(script->buffer_field) {
                buffer[x + y * playfield->width] = col_index;
            } else {
                playfield->cell_field[x + y * playfield->width] = col_index;
            }
            playfield->field[x + y * playfield->width] = script->cell_types[col_index];
        }
    }

    if(script->buffer_field) {
        memcpy(playfield->cell_field, buffer, sizeof(int) * playfield->width * playfield->height);
        free(buffer);
    }
}
