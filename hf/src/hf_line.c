#include "../include/hf_line.h"
#include <math.h>

HF_Vec2f hf_line_closest_point(HF_Line line, HF_Vec2f point) {
    HF_Vec2f line_vec = hf_vec2f_subtract(line.end, line.start);
    float angle = hf_vec2f_angle(line_vec);

    //rotate line and vector so they are aligned horizontally
    float rotation_sin = sinf(-angle);
    float rotation_cos = cosf(-angle);
    HF_Line line_rot = {
        hf_vec2f_rotate_cached(line.start, rotation_sin, rotation_cos),
        hf_vec2f_rotate_cached(line.end, rotation_sin, rotation_cos),
    };
    HF_Vec2f point_rot = hf_vec2f_rotate(point, -angle);

    float line_max_x = line_rot.start.x > line_rot.end.x ? line_rot.start.x : line_rot.end.x;
    float line_min_x = line_rot.start.x <= line_rot.end.x ? line_rot.start.x : line_rot.end.x;
    HF_Vec2f aligned_point = {
        point_rot.x > line_max_x ?
            line_max_x :
            (point_rot.x < line_min_x ? line_min_x : point_rot.x),
        line_rot.start.y,
    };
    return hf_vec2f_rotate_cached(aligned_point, -rotation_sin, rotation_cos);//return point rotated back to original space
}
