#ifndef HF_LINE_H
#define HF_LINE_H

#include <stdbool.h>
#include "hf_vec.h"

typedef struct HF_Line_t {
    HF_Vec2f start;
    HF_Vec2f end;
} HF_Line;

HF_Vec2f hf_line_closest_point(HF_Line line, HF_Vec2f point);

#endif//HF_LINE_H
