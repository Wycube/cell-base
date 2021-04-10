#ifndef SCRIPT_H
#define SCRIPT_H

#include <lua.h>
#include <stdint.h>

typedef struct {
    const char *file_path;
    lua_State *L;

    int num_cell_types;
    uint32_t *cell_types;
    //Some other configuration data
} Script;

Script get_script(const char *file_path);
void free_script(Script *script);
void run_cell_function(Script *script, uint32_t *field, int *cell_field, uint32_t width, uint32_t height);

#endif //SCRIPT_H
