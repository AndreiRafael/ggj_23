#include "../include/hf_vec.h"
#include <math.h>

//HF_Vec2f
HF_Vec2f hf_vec2f_add(HF_Vec2f a, HF_Vec2f b) {
    return (HF_Vec2f) {
        a.x + b.x,
        a.y + b.y
    };
}

HF_Vec2f hf_vec2f_subtract(HF_Vec2f a, HF_Vec2f b) {
    return (HF_Vec2f){
        a.x - b.x,
        a.y - b.y
    };
}

HF_Vec2f hf_vec2f_multiply(HF_Vec2f vec, float scalar) {
    return (HF_Vec2f){
        vec.x * scalar,
        vec.y * scalar
    };
}

HF_Vec2f hf_vec2f_divide(HF_Vec2f vec, float scalar) {
    return (HF_Vec2f){
        vec.x / scalar,
        vec.y / scalar
    };
}

HF_Vec2f hf_vec2f_rotate(HF_Vec2f vec, float rad) {
    float sin_rad = sinf(rad);
    float cos_rad = cosf(rad);
    return hf_vec2f_rotate_cached(vec, sin_rad, cos_rad);
}

HF_Vec2f hf_vec2f_rotate_cached(HF_Vec2f vec, float sin_rad, float cos_rad) {
    return (HF_Vec2f) {
        vec.x * cos_rad - vec.y * sin_rad,
        vec.y * cos_rad + vec.x * sin_rad
    };
}

HF_Vec2f hf_vec2f_lerp(HF_Vec2f a, HF_Vec2f b, float factor) {
    return hf_vec2f_add(
        hf_vec2f_multiply(a, 1.f - factor),
        hf_vec2f_multiply(b, factor)
    );
}

HF_Vec2f hf_vec2f_normalize(HF_Vec2f vec) {
    float mag = hf_vec2f_magnitude(vec);
    if(mag > 0.f) {
        return hf_vec2f_divide(vec, mag);
    }
    return (HF_Vec2f){ 1.f, 0.f };
}

float hf_vec2f_sqr_magnitude(HF_Vec2f vec) {
    return vec.x * vec.x + vec.y * vec.y;
}

float hf_vec2f_magnitude(HF_Vec2f vec) {
    return sqrtf(hf_vec2f_sqr_magnitude(vec));
}

float hf_vec2f_dot(HF_Vec2f a, HF_Vec2f b) {
    return a.x * b.x + a.y * b.y;
}

float hf_vec2f_angle(HF_Vec2f vec) {
    return atan2f(vec.y, vec.x);
}


//HF_Vec2i
HF_Vec2i hf_vec2i_add(HF_Vec2i a, HF_Vec2i b) {
    return (HF_Vec2i){
        a.x + b.x,
        a.y + b.y
    };
}

HF_Vec2i hf_vec2i_subtract(HF_Vec2i a, HF_Vec2i b) {
    return (HF_Vec2i){
        a.x - b.x,
        a.y - b.y
    };
}

HF_Vec2i hf_vec2i_multiply(HF_Vec2i vec, int scalar) {
    return (HF_Vec2i){
        vec.x * scalar,
        vec.y * scalar
    };
}

HF_Vec2i hf_vec2i_divide(HF_Vec2i vec, int scalar) {
    return (HF_Vec2i){
        vec.x / scalar,
        vec.y / scalar
    };
}

int hf_vec2i_sqr_magnitude(HF_Vec2i vec) {
    return vec.x * vec.x + vec.y * vec.y;
}

float hf_vec2i_magnitude(HF_Vec2i vec) {
    return sqrtf((float)hf_vec2i_sqr_magnitude(vec));
}

int hf_vec2i_dot(HF_Vec2i a, HF_Vec2i b) {
    return a.x * b.x + a.y * b.y;
}

float hf_vec2i_angle_rad(HF_Vec2i vec) {
    return atan2f((float)vec.y, (float)vec.x);
}
