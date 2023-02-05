#ifndef VINE_H
#define VINE_H

#include <stdbool.h>

#include "hf_vec.h"
#include "SDL2/SDL.h"

#define VINE_MAX_POINTS 1000
#define VINE_EXPAND_DISTANCE 15.f

typedef struct Vine_s {
    HF_Vec2f position;
    float angle;
    HF_Vec2f points[VINE_MAX_POINTS];
    int point_count;
} Vine;

typedef struct VineInput_s {
    float turn;
} VineInput;

void vine_reset(Vine* vine);
HF_Vec2f vine_next_point(Vine* vine);
void vine_draw(Vine* vine, SDL_Renderer* renderer, SDL_Texture* texture, int offset_y, HF_Vec2f offset);
void vine_process_input(Vine* vine, VineInput input, float turn_multiplier, float delta);
void vine_expand(Vine* vine);

bool vine_collision_self(Vine* vine, HF_Vec2f* hit_point);

#endif//VINE_H
