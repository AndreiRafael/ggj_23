#include "../include/hf_transform.h"

HF_Transform transform_apply(HF_Transform transform, HF_Transform target) {
    (void)transform;
    (void)target;
    return target;// TODO:
}

HF_Vec2f transform_apply_to_point(HF_Transform transform, HF_Vec2f target) {
    return (HF_Vec2f) {
        target.x + transform.position.x,
        target.y + transform.position.y,
    };
}

HF_Triangle transform_apply_to_triangle(HF_Transform transform, HF_Triangle target) {
    (void)transform;
    (void)target;

    return (HF_Triangle){ 
        { 0.f, 0.f },
        { 0.f, 0.f },
        { 0.f, 0.f },
    }; // TODO:
}
