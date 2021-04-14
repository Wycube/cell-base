#include "Utility.h"

#include <math.h>

float max(float a, float b) {
    return a > b ? a : b;
}

float min(float a, float b) {
    return a < b ? a : b;
}

float clamp(float low, float high, float val) {
    return min(max(low, val), high);
}

uint32_t frgb_to_int(float r, float g, float b) {
    //Normalize the components
    float rr = clamp(0.0f, 1.0f, r);
    float gg = clamp(0.0f, 1.0f, g);
    float bb = clamp(0.0f, 1.0f, b);

    return rgb_to_int((uint8_t)(rr * 255), (uint8_t)(gg * 255), (uint8_t)(bb * 255));
}

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
