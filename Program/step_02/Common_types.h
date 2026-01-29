#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H


#include <cstdint>

typedef struct {
    uint16_t *frames;
    uint16_t width;
    uint16_t height;
    uint16_t count;
} IconSequence;
#endif