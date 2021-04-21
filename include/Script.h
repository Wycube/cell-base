#ifndef SCRIPT_H
#define SCRIPT_H

#include <lua.h>
#include <stdint.h>

#include "Playfield.h"

typedef int bool;
#define true 1
#define false 0

//Edge_Case
#define CELL 0
#define WRAP 1

//Neighborhood
#define MOORE 0
#define VON_NEUMANN 1
#define VON_NEUMANN_EXT 2
#define MOORE_VON_NEUMANN 3
#define MORGOLUS 4
#define NONE 5

//Automata Type
#define REGULAR 0
#define BLOCK 1

typedef struct {
    const char *file_path;
    lua_State *L;

    //State
    uint32_t generation;
    int type;

    //Cell types
    int num_cell_types;
    uint32_t *cell_types;
    
    //Options (specified by lua file)
    bool opt_buffer_field;
    int opt_edge_case;
    int opt_edge_cell_type;
    int opt_field_width, opt_field_height;
    //Dimensions (1 or 2)
    int opt_dimensions;
    //Neighborhood (moore, von_neumann, von_neumann_ext, moore_von_neumann, morgolus)
    int opt_neighborhood;

    //Parameter Options (specified by lua file)
    bool param_position;
    bool param_generation;
} Script;

Script get_script(const char *file_path);
void free_script(Script *script);
void run_cell_update(Script *script, Playfield *playfield);

#endif //SCRIPT_H
