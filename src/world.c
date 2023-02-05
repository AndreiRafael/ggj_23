#include "world.h"
#include "hf_vec.h"

static void fill_circle(SDL_Renderer* renderer, HF_Vec2f center, int radius) {
    float width = (float)radius;
    float radius_sqr = (float)(radius * radius);

    for(int i = 0; i < radius; i++) {
        HF_Vec2f point = hf_vec2f_add(center, (HF_Vec2f) { width, (float)i });
        while(hf_vec2f_sqr_magnitude(hf_vec2f_subtract(point, center)) > radius_sqr) {
            width -= 1.f;
            point = hf_vec2f_add(center, (HF_Vec2f) { (float)width, (float)i });
        }

        SDL_RenderDrawLine(
            renderer,
            (int)(center.x - width), (int)(center.y + (float)i),
            (int)(center.x + width), (int)(center.y + (float)i)
        );
        SDL_RenderDrawLine(
            renderer,
            (int)(center.x - width), (int)(center.y - (float)i),
            (int)(center.x + width), (int)(center.y - (float)i)
        );
    }
}

void world_init(World* world, SDL_Renderer* renderer, int w, int h) {
    SDL_Texture* prev_target = SDL_GetRenderTarget(renderer);

    world->w = w;
    world->h = h;

    world->fg_ground = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetTextureBlendMode(world->fg_ground, SDL_BLENDMODE_BLEND);
    world->fg_sky = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetTextureBlendMode(world->fg_sky, SDL_BLENDMODE_BLEND);

    world->bg_ground = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetTextureBlendMode(world->bg_ground, SDL_BLENDMODE_BLEND);
    world->bg_sky = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetTextureBlendMode(world->bg_sky, SDL_BLENDMODE_BLEND);

    world->mask_ground = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetTextureBlendMode(world->mask_ground, SDL_BLENDMODE_MOD);
    world->mask_sky = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetTextureBlendMode(world->mask_sky, SDL_BLENDMODE_MOD);

    world->composed_ground = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetTextureBlendMode(world->composed_ground, SDL_BLENDMODE_ADD);
    world->composed_sky = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetTextureBlendMode(world->composed_sky, SDL_BLENDMODE_ADD);

    world->composed_all = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetTextureBlendMode(world->composed_all, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(renderer, prev_target);
}

void world_deinit(World* world) {
    SDL_DestroyTexture(world->fg_ground);
    SDL_DestroyTexture(world->fg_sky);

    SDL_DestroyTexture(world->bg_ground);
    SDL_DestroyTexture(world->bg_sky);

    SDL_DestroyTexture(world->mask_ground);
    SDL_DestroyTexture(world->mask_sky);

    SDL_DestroyTexture(world->composed_ground);
    SDL_DestroyTexture(world->composed_sky);

    SDL_DestroyTexture(world->composed_all);
}

void world_generate(World* world, SDL_Renderer* renderer) {
    SDL_Texture* prev_target = SDL_GetRenderTarget(renderer);

    SDL_SetRenderTarget(renderer, world->mask_ground);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderTarget(renderer, world->mask_sky);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    world->bubble_count = 0;
    for(int i = 0; i < WORLD_NUM_CLUSTERS; i++) {
        HF_Vec2f bubble_position = { (float)(rand() % world->w), (float)(rand() % world->h) };
        int num_bubbles = (rand() % (WORLD_MAX_SIZE_CLUSTER - WORLD_MIN_SIZE_CLUSTER)) + 1 + WORLD_MIN_SIZE_CLUSTER;

        for(int j = 0; j < num_bubbles; j++) {
            int size = (rand() % (WORLD_MAX_SIZE_HOLE - WORLD_MIN_SIZE_HOLE)) + WORLD_MIN_SIZE_HOLE;

            world->bubbles[world->bubble_count] = (HF_Circle) { .position = bubble_position, .radius = (float)size };
            world->bubble_count++;

            bubble_position = hf_vec2f_add(
                bubble_position,
                (HF_Vec2f) { (float)(rand() % (size * 2)) - (float)size, (float)(rand() % (size * 2)) - (float)size }
            );
        }
    }

    //black to ground_tex
    SDL_SetRenderTarget(renderer, world->mask_ground);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for(int i = 0; i < world->bubble_count; i++) {
        HF_Circle bubble = world->bubbles[i];
        fill_circle(renderer, bubble.position, (int)bubble.radius);
    }

    //white to sky_tex
    SDL_SetRenderTarget(renderer, world->mask_sky);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for(int i = 0; i < world->bubble_count; i++) {
        HF_Circle bubble = world->bubbles[i];
        fill_circle(renderer, bubble.position, (int)bubble.radius);
    }

    SDL_SetRenderTarget(renderer, prev_target);
}

void world_clear(World* world, SDL_Renderer* renderer) {
    SDL_BlendMode prev_mode;
    SDL_GetRenderDrawBlendMode(renderer, &prev_mode);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    SDL_SetRenderTarget(renderer, world->bg_ground);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderTarget(renderer, world->bg_sky);
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderTarget(renderer, world->fg_ground);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    SDL_SetRenderTarget(renderer, world->fg_sky);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);


    SDL_SetRenderDrawBlendMode(renderer, prev_mode);
}

void world_compose_texture(World* world, SDL_Renderer* renderer) {
    SDL_Texture* prev_target = SDL_GetRenderTarget(renderer);
    SDL_BlendMode prev_mode;
    SDL_GetRenderDrawBlendMode(renderer, &prev_mode);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    //ground
    SDL_SetRenderTarget(renderer, world->composed_ground);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, world->bg_ground, NULL, NULL);
    SDL_RenderCopy(renderer, world->fg_ground, NULL, NULL);
    SDL_RenderCopy(renderer, world->mask_ground, NULL, NULL);

    //sky
    SDL_SetRenderTarget(renderer, world->composed_sky);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, world->bg_sky, NULL, NULL);
    SDL_RenderCopy(renderer, world->fg_sky, NULL, NULL);
    SDL_RenderCopy(renderer, world->mask_sky, NULL, NULL);

    //all
    SDL_SetRenderTarget(renderer, world->composed_all);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, world->composed_ground, NULL, NULL);
    SDL_RenderCopy(renderer, world->composed_sky, NULL, NULL);

    SDL_SetRenderDrawBlendMode(renderer, prev_mode);
    SDL_SetRenderTarget(renderer, prev_target);
}

bool world_point_is_in_bubble(World* world, HF_Vec2f point) {
    for(int i = 0; i < world->bubble_count; i++) {
        HF_Circle bubble = world->bubbles[i];
        HF_Vec2f vec = hf_vec2f_subtract(bubble.position, point);
        if(hf_vec2f_sqr_magnitude(vec) < bubble.radius * bubble.radius) {
            return true;
        }
    }
    return false;
}

bool world_point_is_off_world(World* world, HF_Vec2f point) {
    return
        point.x < 0.f ||
        point.y < 0.f ||
        point.x > (float)world->w ||
        point.y > (float)world->h
    ;
}
