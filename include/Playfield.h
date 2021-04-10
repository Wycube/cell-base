#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include <stdint.h>

//An array containing colors as 32-bit unsigned integers.
//The Playfield is a rectangle in which each integer is a cell.
typedef struct {
    uint32_t *field, width, height;
    int *cell_field;

} Playfield;

Playfield create_playfield(uint32_t width, uint32_t height);
void destroy_playfield(Playfield *playfield);

#endif //PLAYFIELD_H
