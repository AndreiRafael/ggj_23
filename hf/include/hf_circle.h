#ifndef HF_CIRCLE_H
#define HF_CIRCLE_H

#include "hf_vec.h"
#include <stdbool.h>

typedef struct HF_Circle_s {
    HF_Vec2f position;
    float radius;
} HF_Circle;

#endif//HF_CIRCLE_H
