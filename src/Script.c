#include "Script.h"

#include <lualib.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>

#include "Utility.h"

Script get_script(const char *file_path) {
    lua_State *state = luaL_newstate();
    luaL_openlibs(state);

    luaL_dofile(state, file_path);

    lua_getglobal(state, "num_cell_types");
    if(!lua_isnumber(state, -1)) {
        printf("num_cell_types not specified\n");
        exit(-1);
    }
    int num_cell_types = lua_tointeger(state, -1);
    printf("num_cell_types = %i\n", num_cell_types);
    lua_pop(state, 1);

    uint32_t *cell_types = (uint32_t*)malloc(sizeof(uint32_t) * num_cell_types);

    int i;
    char buffer[20];
    for(i = 0; i < num_cell_types; i++) {
        sprintf(buffer, "cell_type_%i", i);
        
        lua_getglobal(state, buffer);
        if(!lua_istable(state, -1)) {
            printf("%s not specified\n", buffer);
            exit(-1);
        }
        float r, g, b;
        lua_pushnumber(state, 1);
        lua_gettable(state, -2);
        r = lua_tonumber(state, -1);
        lua_pop(state, 1);
        
        lua_pushnumber(state, 2);
        lua_gettable(state, -2);
        g = lua_tonumber(state, -1);
        lua_pop(state, 1);

        lua_pushnumber(state, 3);
        lua_gettable(state, -2);
        b = lua_tonumber(state, -1);
        lua_pop(state, 2);

        cell_types[i] = frgb_to_int(r, g, b);

        printf("%s = (%.2f, %.2f, %.2f)\n", buffer, r, g, b);
    }

    Script s = {file_path, state, num_cell_types, cell_types};

    return s;
}

//I should check for null here
void free_script(Script *script) {
    free(script->cell_types);
}

static void stackDump (lua_State *L) {
      printf("Stack: ");
      
      int i;
      int top = lua_gettop(L);

        printf("Size:%i ", top);

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

//TODO: Add another edge case where the cells wrap around
void push_number(lua_State *L, int *cell_field, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    if(x >= 0 && y >= 0 && x < width && y < height) {
        lua_pushinteger(L, cell_field[x + y * width]);
    } else {
        lua_pushinteger(L, 0);
    }
}

void run_cell_function(Script *script, uint32_t *field, int *cell_field, uint32_t width, uint32_t height) {
    int *buffer = (int*)malloc(sizeof(int) * width * height);
    memcpy(buffer, cell_field, sizeof(int) * width * height);

    int col_index;
    
    luaL_dofile(script->L, script->file_path);

    for(uint32_t x = 0; x < width; x++) {
        for(uint32_t y = 0; y < height; y++) {
            //Push function onto the stack
            lua_getglobal(script->L, "update_cell");

            if(!lua_isfunction(script->L, -1)) {
                printf("Function doesn't work for some reason, ????\n");
            }

            //Push parameters onto the stack
            //nw, nn, ne, ww, cc, ee, sw, ss, se
            push_number(script->L, cell_field, (x - 1), (y - 1), width, height);
            push_number(script->L, cell_field, (x - 0), (y - 1), width, height);
            push_number(script->L, cell_field, (x + 1), (y - 1), width, height);
            push_number(script->L, cell_field, (x - 1), (y - 0), width, height);
            push_number(script->L, cell_field, (x - 0), (y - 0), width, height);
            push_number(script->L, cell_field, (x + 1), (y - 0), width, height);
            push_number(script->L, cell_field, (x - 1), (y + 1), width, height);
            push_number(script->L, cell_field, (x - 0), (y + 1), width, height);
            push_number(script->L, cell_field, (x + 1), (y + 1), width, height);

            if(lua_pcall(script->L, 9, 1, 0) != 0) {
                printf("Fail: ");
                exit(-1);
                stackDump(script->L);
                //printf("It failed: %s\n", lua_tostring(script->L, 1));
                //lua_pop(script->L, 1);
                return;
            }

            //Get the table that was returned
            col_index = lua_tointeger(script->L, -1);
            lua_pop(script->L, 1);
            
            buffer[x + y * width] = col_index;
            field[x + y * width] = script->cell_types[col_index];
        }
    }

    memcpy(cell_field, buffer, sizeof(int) * width * height);
    free(buffer);
}
