#include "Utility.h"

//Converts 3 color components, rgb, in the range 0-255 into a single unsigned integer
uint32_t rgb_to_int(uint8_t r, uint8_t g, uint8_t b) {
    return (r << 24) | (g << 16) | (b << 8) | 0xff;
}

void int_to_vec4(float *vec4, uint32_t color) {
    uint8_t r = color >> 24;
    uint8_t g = color >> 16 & 0xff;
    uint8_t b = color >> 8 & 0xff;
    uint8_t a = color & 0xff;

    vec4[0] = (float)r / 255.0f;
    vec4[1] = (float)g / 255.0f;
    vec4[2] = (float)b / 255.0f;
    vec4[3] = (float)a / 255.0f;
}
