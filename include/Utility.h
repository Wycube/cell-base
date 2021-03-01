#ifndef UTILITY_H
#define UTILITY_H

#include <stdint.h>

uint32_t rgb_to_int(uint8_t r, uint8_t g, uint8_t b);
void int_to_vec4(float *vec4, uint32_t color);

#endif //UTILITY_H
