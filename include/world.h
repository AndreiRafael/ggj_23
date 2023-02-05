#ifndef WORLD_H
#define WORLD_H

#include <time.h>

#include "SDL2/SDL.h"
#include "hf_circle.h"

#define WORLD_NUM_CLUSTERS 6
#define WORLD_MIN_SIZE_CLUSTER 5
#define WORLD_MAX_SIZE_CLUSTER 15

#define WORLD_MIN_SIZE_HOLE 20
#define WORLD_MAX_SIZE_HOLE 50

typedef struct World_s {
    int w;
    int h;

    SDL_Texture* bg_ground;
    SDL_Texture* bg_sky;

    SDL_Texture* fg_ground;
    SDL_Texture* fg_sky;

    SDL_Texture* mask_ground;
    SDL_Texture* mask_sky;

    SDL_Texture* composed_ground;
    SDL_Texture* composed_sky;

    SDL_Texture* composed_all;

    HF_Circle bubbles[WORLD_MAX_SIZE_CLUSTER * WORLD_NUM_CLUSTERS];
    int bubble_count;
} World;

void world_init(World* world, SDL_Renderer* renderer, int w, int h);
void world_deinit(World* world);

void world_generate(World* world, SDL_Renderer* renderer);

void world_clear(World* world, SDL_Renderer* renderer);
void world_compose_texture(World* world, SDL_Renderer* renderer);

bool world_point_is_in_bubble(World* world, HF_Vec2f point);
bool world_point_is_off_world(World* world, HF_Vec2f point);

#endif//WORLD_H
