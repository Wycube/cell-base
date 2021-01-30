#include "Utility.h"

//Converts 3 color components, rgb, in the range 0-255 into a single unsigned integer
uint32_t rgb_to_int(uint8_t r, uint8_t g, uint8_t b) {
    return (r << 24) | (g << 16) | (b << 8) | 0xff;
}