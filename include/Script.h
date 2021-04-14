#ifndef SCRIPT_H
#define SCRIPT_H

#include <lua.h>
#include <stdint.h>

#include "Playfield.h"

typedef int bool;

#define true 1
#define false 0

typedef struct {
    const char *file_path;
    lua_State *L;

    //Cell types
    int num_cell_types;
    uint32_t *cell_types;
    
    //Options (specified by lua file)
    bool buffer_field;
    const char *edge_case;
    int field_width, field_height;

    //Parameter Options (specified by lua file)
    bool position;
    bool generation;

    //Dimensions (1 or 2)
    int dimensions;

    //Neighborhood (moore, von_neumann, von_neumann_ext, moore_von_neumann, morgolus)
    const char *neighborhood;
} Script;

Script get_script(const char *file_path);
void free_script(Script *script);
void run_cell_update(Script *script, Playfield *playfield);

#endif //SCRIPT_H
