#include "vine.h"
#include "hf_line.h"
#include "hf_intersection.h"

void vine_reset(Vine* vine) {
    vine->point_count = 0;
    vine->angle = (float)M_PI / 2.f;
}

HF_Vec2f vine_next_point(Vine* vine) {
    HF_Vec2f expand_dir = { VINE_EXPAND_DISTANCE, 0.f };
    expand_dir = hf_vec2f_rotate(expand_dir, vine->angle);

    return hf_vec2f_add(vine->position, expand_dir);
}

static void vine__draw_segment(SDL_Renderer* renderer, SDL_Texture* texture, int tex_offset_y, int offset_x, HF_Vec2f start, HF_Vec2f end) {
    HF_Vec2f mid_point = hf_vec2f_divide(hf_vec2f_add(start, end), 2.f);

    SDL_Rect src_rect = {
        offset_x,
        tex_offset_y,
        21,
        21
    };
    SDL_Rect dest_rect = {
        (int)mid_point.x - 10,
        (int)mid_point.y - 10,
        21,
        21
    };
    float deg = hf_vec2f_angle(hf_vec2f_subtract(start, end)) * (float)(180.0 / M_PI);
    SDL_RenderCopyEx(renderer, texture, &src_rect, &dest_rect, deg - 90.f, NULL, SDL_FLIP_NONE);
}

void vine_draw(Vine* vine, SDL_Renderer* renderer, SDL_Texture* texture, int tex_offset_y, HF_Vec2f offset) {
    if(vine_collision_self(vine, NULL)){
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    }

    for(int i = 1; i < vine->point_count; i++) {
        HF_Vec2f prev_point = hf_vec2f_add(offset, vine->points[i - 1]);
        HF_Vec2f this_point = hf_vec2f_add(offset, vine->points[i]);

        int val = i % 6;//rand() % 4;
        vine__draw_segment(renderer, texture, tex_offset_y, val * 21, prev_point, this_point);
    }

    //desenha parte movel do cipo
    {
        HF_Vec2f expand_dir = { VINE_EXPAND_DISTANCE, 0.f };
        expand_dir = hf_vec2f_rotate(expand_dir, vine->angle);

        HF_Vec2f next_pos = hf_vec2f_add(offset, hf_vec2f_add(vine->position, expand_dir));
        HF_Vec2f vine_pos = hf_vec2f_add(offset, vine->position);

        int val = vine->point_count % 6;//rand() % 4;
        vine__draw_segment(renderer, texture, tex_offset_y, val * 21, vine_pos, next_pos);
    }
}

void vine_process_input(Vine* vine, VineInput input, float turn_multiplier, float delta) {
    vine->angle += input.turn * turn_multiplier * delta;
}

void vine_expand(Vine* vine) {
    HF_Vec2f vec = { VINE_EXPAND_DISTANCE, 0.f };
    vec = hf_vec2f_rotate(vec, vine->angle);

    if(vine->point_count == 0) {//needs to add starting position as a point
        vine->points[0] = vine->position;
        vine->point_count++;
    }

    if(vine->point_count >= VINE_MAX_POINTS) {//mover todos points um indice abaixo
        for(int i = 1; i < VINE_MAX_POINTS; i++) {
            vine->points[i - 1] = vine->points[i];
        }
    }
    else {
        vine->point_count++;
    }

    vine->points[vine->point_count - 1] = vine->position = hf_vec2f_add(vine->position, vec);
}

static void vine__get_lines(Vine* vine, HF_Line* lines) {
    for(int i = 1; i < vine->point_count; i++) {
        HF_Vec2f this_point = vine->points[i];
        HF_Vec2f prev_point = vine->points[i - 1];

        lines[i - 1] = (HF_Line) {
            .start = prev_point,
            .end   = this_point
        };
    }
}

bool vine_collision_self(Vine* vine, HF_Vec2f* hit_point) {
    HF_Line vine_lines[VINE_MAX_POINTS - 1];
    vine__get_lines(vine, vine_lines);

    int line_count = vine->point_count - 1;

    //só checa colisão do ponto frontal, não checa colisão com a última linha
    for(int i = 0; i < line_count - 1; i++) {
        HF_Line front_line = { vine->position, vine_next_point(vine) };
        HF_Line other_line = vine_lines[i];

        if(hf_intersection_lines(front_line, other_line, hit_point)) {
            return true;
        }
    }
    return false;
}
