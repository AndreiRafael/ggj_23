#ifndef HF_TRANSFORM_H
#define HF_TRANSFORM_H

#include "hf_triangle.h"

typedef struct HF_Transform_t {
    HF_Vec2f position;
    HF_Vec2f scale;
    float rotation;
} HF_Transform;

HF_Transform transform_apply(HF_Transform transform, HF_Transform target);
HF_Vec2f     transform_apply_to_point(HF_Transform transform, HF_Vec2f target);
HF_Triangle  transform_apply_to_triangle(HF_Transform transform, HF_Triangle target);

#endif//HF_TRANSFORM_H
