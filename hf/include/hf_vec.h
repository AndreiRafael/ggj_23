#ifndef HF_VEC_H
#define HF_VEC_H

typedef struct HF_Vec2f_t {
    float x;
    float y;
} HF_Vec2f;

typedef struct HF_Vec2i_t {
    int x;
    int y;
} HF_Vec2i;

HF_Vec2f hf_vec2f_add(HF_Vec2f a, HF_Vec2f b);
HF_Vec2f hf_vec2f_subtract(HF_Vec2f a, HF_Vec2f b);
HF_Vec2f hf_vec2f_multiply(HF_Vec2f vec, float scalar);
HF_Vec2f hf_vec2f_divide(HF_Vec2f vec, float scalar);
HF_Vec2f hf_vec2f_rotate(HF_Vec2f vec, float rad);
HF_Vec2f hf_vec2f_rotate_cached(HF_Vec2f vec, float sin_rad, float cos_rad);
HF_Vec2f hf_vec2f_lerp(HF_Vec2f a, HF_Vec2f b, float factor);
HF_Vec2f hf_vec2f_normalize(HF_Vec2f vec);
float    hf_vec2f_magnitude(HF_Vec2f vec);
float    hf_vec2f_sqr_magnitude(HF_Vec2f vec);
float    hf_vec2f_dot(HF_Vec2f a, HF_Vec2f b);
float    hf_vec2f_angle(HF_Vec2f vec);

HF_Vec2i hf_vec2i_add(HF_Vec2i a, HF_Vec2i b);
HF_Vec2i hf_vec2i_subtract(HF_Vec2i a, HF_Vec2i b);
HF_Vec2i hf_vec2i_multiply(HF_Vec2i vec, int scalar);
HF_Vec2i hf_vec2i_divide(HF_Vec2i vec, int scalar);
int      hf_vec2i_sqr_magnitude(HF_Vec2i vec);
float    hf_vec2i_magnitude(HF_Vec2i vec);
int      hf_vec2i_dot(HF_Vec2i a, HF_Vec2i b);
float    hf_vec2i_angle(HF_Vec2i vec);

#endif//HF_VEC_H
