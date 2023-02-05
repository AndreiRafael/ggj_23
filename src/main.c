#include <stdio.h>
#include <stdbool.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#include "SDL2/SDL_ttf.h"

#include "hf_vec.h"
#include "hf_line.h"
#include "hf_triangle.h"
#include "hf_circle.h"
#include "hf_intersection.h"

#include "vine.h"
#include "world.h"

#define WIN_W 1920
#define WIN_H 1080

#define MAX_SPEED 5.f
#define SPEEDBAR_W 400
#define SPEEDBAR_H 15

void draw_line(SDL_Renderer* renderer, HF_Line line) {
    SDL_RenderDrawLineF(
        renderer,
        roundf(line.start.x),
        roundf(line.start.y),
        roundf(line.end.x),
        roundf(line.end.y)
    );
}

void draw_vec(SDL_Renderer* renderer, float x, float y, HF_Vec2f vec) {
    SDL_RenderDrawLineF(
        renderer,
        x,
        y,
        x + vec.x * 20.f,
        y - vec.y * 20.f
    );
}

void draw_point(SDL_Renderer* renderer, HF_Vec2f point) {
    SDL_FRect rect = {
        point.x - 5.f,
        point.y - 5.f,
        10.f,
        10.f
    };

    SDL_RenderFillRectF(renderer, &rect);
}

void draw_circle(SDL_Renderer* renderer, HF_Vec2f center, int radius) {
    HF_Vec2f vec = { .x = (float)radius, 0.f };

    float angle_delta = (2 * (float)M_PI) / (float)60;
    for(int i = 0; i < 60; i++) {
        float angle1 = angle_delta * (float)i;
        float angle2 = angle_delta * (float)(i + 1);

        HF_Line line = {
            .start = hf_vec2f_add(center, hf_vec2f_rotate(vec, angle1)),
            .end   = hf_vec2f_add(center, hf_vec2f_rotate(vec, angle2)),
        };
        draw_line(renderer, line);
    }
}

void draw_triangle(SDL_Renderer* renderer, HF_Triangle triangle) {
    HF_Line triangle_edges[3];
    hf_triangle_get_edges(triangle, triangle_edges);
    for(int i = 0; i < 3; i++) {
        draw_line(renderer, triangle_edges[i]);
    }
}

void change_color_if(SDL_Renderer* renderer, bool condition) {
    if(condition) {
        SDL_SetRenderDrawColor(renderer,   0,   0, 255, 255);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    }
}

SDL_Texture* load_texture(SDL_Renderer* renderer, const char* path, bool use_keying) {
    SDL_Texture* tex = NULL;
    SDL_Surface* surf = SDL_LoadBMP(path);
    if(surf) {
        if(use_keying) {
            SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 255, 0, 255));
        }
        tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
    }

    return tex;
}

void draw_tiled(SDL_Renderer* renderer, SDL_Texture* texture, int pos_x, int pos_y, int repeat_x, int repeat_y) {
    int tex_w;
    int tex_h;
    SDL_QueryTexture(texture, NULL, NULL, &tex_w, &tex_h);

    for(int x = 0; x < repeat_x; x++) {
        for(int y = 0; y < repeat_y; y++) {
            SDL_Rect dest_rect = {
                pos_x + x * tex_w,
                pos_y + y * tex_h,
                tex_w,
                tex_h,
            };
            SDL_RenderCopy(renderer, texture, NULL, &dest_rect);
        }
    }
}

void update_font_texture(SDL_Texture** texture_ptr, SDL_Renderer* renderer, TTF_Font* font, const char* text) {
    if(*texture_ptr) {
        SDL_DestroyTexture(*texture_ptr);
    }
    SDL_Surface* surf = TTF_RenderText_Solid(font, text, (SDL_Color) { 255, 255, 255, 255 });
    if(surf) {
        *texture_ptr = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
    }
}

typedef struct AssetData_s {
    SDL_Texture* tex_plants;
    SDL_Texture* tex_ground;
    SDL_Texture* tex_water;

    Mix_Chunk* leaves_chunks[5];

    Mix_Music* music_fast;

    TTF_Font* font_score;
    TTF_Font* font_title;

    SDL_Texture* text_play_score;
    SDL_Texture* text_play_best_score;

    SDL_Texture* text_start_title;
} AssetData;

void asset_data_init(AssetData* asset_data, SDL_Renderer* renderer) {
    asset_data->tex_plants = load_texture(renderer, "./assets/sprites/plants.bmp", true);
    asset_data->tex_ground = load_texture(renderer, "./assets/sprites/ground.bmp", false);
    asset_data->tex_water = load_texture(renderer, "./assets/sprites/water.bmp", false);

    asset_data->leaves_chunks[0] = Mix_LoadWAV("./assets/sfx/leaves00.wav");
    asset_data->leaves_chunks[1] = Mix_LoadWAV("./assets/sfx/leaves01.wav");
    asset_data->leaves_chunks[2] = Mix_LoadWAV("./assets/sfx/leaves02.wav");
    asset_data->leaves_chunks[3] = Mix_LoadWAV("./assets/sfx/leaves03.wav");
    asset_data->leaves_chunks[4] = Mix_LoadWAV("./assets/sfx/leaves04.wav");
    for(int i= 0; i < 5; i++) {
        Mix_VolumeChunk(asset_data->leaves_chunks[i], MIX_MAX_VOLUME / 4);
    }

    asset_data->music_fast = Mix_LoadMUS("./assets/music/fast.mp3");

    asset_data->font_score = TTF_OpenFont("./assets/fonts/arial.ttf", 24);
    asset_data->font_title = TTF_OpenFont("./assets/fonts/arial.ttf", 198);

    asset_data->text_play_score = NULL;
    asset_data->text_play_best_score = NULL;

    asset_data->text_start_title = NULL;
    update_font_texture(&asset_data->text_start_title, renderer, asset_data->font_title, "TREPADEIRA");
}

void asset_data_deinit(AssetData* asset_data) {
    SDL_DestroyTexture(asset_data->tex_ground);
    SDL_DestroyTexture(asset_data->tex_plants);
    SDL_DestroyTexture(asset_data->tex_water);

    Mix_FreeMusic(asset_data->music_fast);
    for(int i = 0; i < 5; i++) {
        Mix_FreeChunk(asset_data->leaves_chunks[i]);
    }

    SDL_DestroyTexture(asset_data->text_play_score);
    SDL_DestroyTexture(asset_data->text_play_best_score);
    SDL_DestroyTexture(asset_data->text_start_title);

    TTF_CloseFont(asset_data->font_score);
    TTF_CloseFont(asset_data->font_title);
}

typedef struct GameInput_s {
    VineInput vine_input;
    bool ok;
} GameInput;

void game_input_process_event(GameInput* game_input, SDL_Event e) {
    switch (e.type) {
    case SDL_KEYDOWN:
        switch (e.key.keysym.sym) {
        case SDLK_RETURN:
        case SDLK_RETURN2:
        case SDLK_SPACE:
            game_input->ok = true;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void game_input_process_keyboard(GameInput* game_input, const Uint8* keyboard_state) {
    if(keyboard_state[SDL_SCANCODE_A]) {
        game_input->vine_input.turn -= 1.f;
    }
    if(keyboard_state[SDL_SCANCODE_D]) {
        game_input->vine_input.turn += 1.f;
    }
}

typedef enum GameState_s {
    GAME_STATE_Start,
    GAME_STATE_Play,
    GAME_STATE_Lost,
} GameState;

typedef struct GameData_s {
    Vine vine;
    World world;
    float counter;
    float vine_speed;
    bool vine_go;
    int score;
    int best_score;
    GameState game_state;
} GameData;

void game_data_init(GameData* game_data, SDL_Renderer* renderer) {
    game_data->game_state = GAME_STATE_Start;
    game_data->best_score = -1;
    world_init(&game_data->world, renderer, WIN_W / 2, WIN_H / 2);
}

void game_data_deinit(GameData* game_data) {
    world_deinit(&game_data->world);
}

void game_data_reset(GameData* game_data, SDL_Renderer* renderer) {
    game_data->vine = (Vine) {
        .position = { 200.f, 200.f },
        .point_count = 0,
        .angle = 0.f
    };
    game_data->vine_speed = 2.f;
    game_data->vine_go = false;
    game_data->counter = 0.f;
    game_data->score = 0;

    world_generate(&game_data->world, renderer);
}

void game_data_update_score(GameData* game_data, AssetData* asset_data, SDL_Renderer* renderer) {
    char buff[300];
    sprintf(buff, "PONTOS: %d", game_data->score);

    update_font_texture(&asset_data->text_play_score, renderer, asset_data->font_score, buff);
    if(game_data->score > game_data->best_score) {
        game_data->best_score = game_data->score;

        sprintf(buff, "MELHOR: %d", game_data->best_score);
        update_font_texture(&asset_data->text_play_best_score, renderer, asset_data->font_score, buff);
    }
}

void game_data_switch_game_state(GameData* game_data, AssetData* asset_data, GameState new_state, SDL_Renderer* renderer) {
    //deinit current state

    //init new_state
    switch (new_state) {
    case GAME_STATE_Play:
        game_data_reset(game_data, renderer);
        game_data_update_score(game_data, asset_data, renderer);
        break;
    default:
        break;
    }
    game_data->game_state = new_state;
}

void game_data_update(GameData* game_data, AssetData* asset_data, GameInput input, float delta, SDL_Renderer* renderer) {
    switch (game_data->game_state) {
    case GAME_STATE_Start:
        if(input.ok) {
            game_data_switch_game_state(game_data, asset_data, GAME_STATE_Play, renderer);
        }
        break;
    case GAME_STATE_Play:
        if(input.ok) {
            game_data->vine_go = true;
        }

        if(
            vine_collision_self(&game_data->vine, NULL) ||
            world_point_is_off_world(&game_data->world, game_data->vine.position) ||
            game_data->vine_speed < 0.01
        ) {
            game_data_switch_game_state(game_data, asset_data, GAME_STATE_Start, renderer);
        }

        bool in_bubble = world_point_is_in_bubble(&game_data->world, vine_next_point(&game_data->vine));

        if(game_data->vine_go) {
            game_data->counter += delta * game_data->vine_speed;
            if(game_data->counter >= .3f) {
                game_data->counter -= .3f;
                game_data->score++;
                game_data_update_score(game_data, asset_data, renderer);
                vine_expand(&game_data->vine);
                if((rand() % 4) == 0) {
                    Mix_PlayChannel(-1, asset_data->leaves_chunks[rand() % 5], SDL_FALSE);
                }
            }

            if(in_bubble) {
                game_data->vine_speed += delta * 1.4f;
                if(game_data->vine_speed > MAX_SPEED) {
                    game_data->vine_speed = MAX_SPEED;
                }
            }
            else {
                game_data->vine_speed -= delta * .2f;
                if(game_data->vine_speed < 0.f) {
                    game_data->vine_speed = 0.f;
                }
            }
        }

        float turn_value = in_bubble ? 1.2f : 3.5f;
        vine_process_input(&game_data->vine, input.vine_input, turn_value, delta);
        break;
    default:
        break;
    }
}

void game_data_render(GameData* game_data, AssetData* asset_data, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);
    SDL_RenderClear(renderer);

    world_clear(&game_data->world, renderer);

    //bg ground
    SDL_SetRenderTarget(renderer, game_data->world.bg_ground);
    draw_tiled(renderer,asset_data->tex_ground, 0, 0, 40, 30);
    //fg_ground
    SDL_SetRenderTarget(renderer, game_data->world.fg_ground);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_SetTextureColorMod(asset_data->tex_plants, 150, 150, 150);
    vine_draw(&game_data->vine, renderer, asset_data->tex_plants, 21, (HF_Vec2f) { 0.f, 1.f });
    SDL_SetTextureColorMod(asset_data->tex_plants, 255, 255, 255);
    vine_draw(&game_data->vine, renderer, asset_data->tex_plants, 21, (HF_Vec2f) { 0.f, 0.f });
    //bg sky
    SDL_SetRenderTarget(renderer, game_data->world.bg_sky);
    draw_tiled(renderer, asset_data->tex_water, 0, 0, 20, 20);
    //fg_sky
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_SetRenderTarget(renderer, game_data->world.fg_sky);
    SDL_SetTextureColorMod(asset_data->tex_plants, 150, 150, 150);
    vine_draw(&game_data->vine, renderer, asset_data->tex_plants, 0, (HF_Vec2f) { 0.f, 1.f });
    SDL_SetTextureColorMod(asset_data->tex_plants, 255, 255, 255);
    vine_draw(&game_data->vine, renderer, asset_data->tex_plants, 0, (HF_Vec2f) { 0.f, 0.f });

    SDL_SetRenderTarget(renderer, NULL);
    world_compose_texture(&game_data->world, renderer);
    SDL_RenderCopy(renderer, game_data->world.composed_all, NULL, NULL);

    switch (game_data->game_state) {
    case GAME_STATE_Start: {
        //desenhar trepadeira no meio da tela
        int text_tile_w;
        int text_tile_h;
        SDL_QueryTexture(asset_data->text_start_title, NULL, NULL, &text_tile_w, &text_tile_h);

        SDL_Rect dest_rect = {
            WIN_W / 2 - text_tile_w / 2,
            WIN_H / 2 - text_tile_h / 2,
            text_tile_w,
            text_tile_h,
        };
        SDL_SetTextureColorMod(asset_data->text_start_title, 0, 0, 0);
        SDL_SetTextureAlphaMod(asset_data->text_start_title, 100);
        SDL_RenderCopy(renderer, asset_data->text_start_title, NULL, &dest_rect);
        dest_rect.x += 5;
        dest_rect.y += 5;
        SDL_SetTextureColorMod(asset_data->text_start_title, 150, 255, 150);
        SDL_SetTextureAlphaMod(asset_data->text_start_title, 255);
        SDL_RenderCopy(renderer, asset_data->text_start_title, NULL, &dest_rect);
        break;
    }
    case GAME_STATE_Play: {
        {//render score text
            int text_score_w;
            int text_score_h;
            SDL_QueryTexture(asset_data->text_play_score, NULL, NULL, &text_score_w, &text_score_h);

            SDL_Rect dest_rect = {
                20,
                40 + text_score_h / 2,
                text_score_w,
                text_score_h,
            };
            SDL_RenderCopy(renderer, asset_data->text_play_score, NULL, &dest_rect);
        }
        {//render best score text
            int text_best_score_w;
            int text_best_score_h;
            SDL_QueryTexture(asset_data->text_play_best_score, NULL, NULL, &text_best_score_w, &text_best_score_h);

            SDL_Rect dest_rect = {
                20,
                80 + text_best_score_h / 2,
                text_best_score_w,
                text_best_score_h,
            };
            SDL_RenderCopy(renderer, asset_data->text_play_best_score, NULL, &dest_rect);
        }

        //draw top bar
        {
            SDL_Rect bar_rect = {
                WIN_W / 2 - SPEEDBAR_W / 2,
                20,
                SPEEDBAR_W,
                SPEEDBAR_H,
            };

            float pct = game_data->vine_speed / MAX_SPEED;
            SDL_Rect filled_rect = {
                bar_rect.x,
                bar_rect.y,
                (int)(pct * (float)SPEEDBAR_W),
                bar_rect.h,
            };

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &bar_rect);
            SDL_RenderFillRect(renderer, &filled_rect);
        }
        break;
    }
    default:
        break;
    }
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    srand((unsigned int)time(NULL));

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        exit(EXIT_FAILURE);
    }

    Mix_Init(MIX_INIT_MP3);
    Mix_OpenAudio(48000, AUDIO_S16SYS, 1, 2048);
    Mix_AllocateChannels(20);

    TTF_Init();

    SDL_Window* window = SDL_CreateWindow(
        "Trepadeira",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WIN_W,
        WIN_H,
        SDL_WINDOW_SHOWN
    );
    if(!window) {
        exit(EXIT_FAILURE);
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer) {
        exit(EXIT_FAILURE);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    //SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    GameData game_data;
    game_data_init(&game_data, renderer);
    game_data_reset(&game_data, renderer);

    AssetData asset_data;
    asset_data_init(&asset_data, renderer);

    Mix_PlayMusic(asset_data.music_fast, 1000);

    bool quit = false;
    Uint32 prev_ticks = SDL_GetTicks();
    while(!quit) {
        {//logic update
            //frame variables
            GameInput game_input = {
                .vine_input = {
                    .turn = 0.f,
                },
                .ok = false
            };

            SDL_Event e;
            while(SDL_PollEvent(&e)) {
                if(e.type == SDL_QUIT) {
                    quit = true;
                }
                game_input_process_event(&game_input, e);
            }

            const Uint8* keyboard = SDL_GetKeyboardState(NULL);
            game_input_process_keyboard(&game_input, keyboard);

            Uint32 new_ticks = SDL_GetTicks();
            float delta = (float)(new_ticks - prev_ticks) / 1000.f;
            prev_ticks = new_ticks;

            game_data_update(&game_data, &asset_data, game_input, delta, renderer);
        }

        //drawing loop
        game_data_render(&game_data, &asset_data, renderer);

        SDL_RenderPresent(renderer);
    }

    game_data_deinit(&game_data);
    asset_data_deinit(&asset_data);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    Mix_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}
