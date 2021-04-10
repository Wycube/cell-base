#include "Playfield.h"

#include <string.h>
#include <stdlib.h>

//Allocate the array on the heap and return a Playfield with it.
Playfield create_playfield(uint32_t width, uint32_t height) {
    uint32_t *field = (uint32_t*)malloc(sizeof(uint32_t) * width * height);
    int *cell_field = (int*)malloc(sizeof(int) * width * height);

    //Set all cells to white
    memset(field, 0xffffffff, sizeof(uint32_t) * width * height);
    memset(cell_field, 0, sizeof(int) * width * height);
    
    Playfield playfield = {field, width, height, cell_field};
    return playfield;
}

//Free the array in the playfield struct, then set field to NULL.
void destroy_playfield(Playfield *playfield) {
    free(playfield->field);
    playfield->field = NULL; //Set to null so this memory won't get freed again
}
