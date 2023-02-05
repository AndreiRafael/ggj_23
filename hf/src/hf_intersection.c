#include "../include/hf_intersection.h"
#include <math.h>

bool hf_intersection_lines(HF_Line a, HF_Line b, HF_Vec2f* hit_point) {
    //complex line overlap algorithm:
    //1. rotate lines so that one of them is axis aligned
    //2. check if second line would intersect an infinite fisrt line
    //3. check if would be contact point is really in range of the finite first line
    HF_Vec2f a_vec = hf_vec2f_subtract(a.end, a.start);
    {//use dot to check if a is between the points of b
        HF_Vec2f a_vec90 = { a_vec.y, -a_vec.x };//a vector but rotated by 90 degrees

        bool dot1 = hf_vec2f_dot(a_vec90, hf_vec2f_subtract(b.start, a.start)) > 0.f;
        bool dot2 = hf_vec2f_dot(a_vec90, hf_vec2f_subtract(b.end, a.start)) > 0.f;

        if(dot1 == dot2) {//it both dot products are the same signal, it means no intersection can occur
            return false;
        }
    }

    HF_Vec2f b_vec = hf_vec2f_subtract(b.end, b.start);
    {//use dot to check if b is between the points of a
        HF_Vec2f b_vec90 = { b_vec.y, -b_vec.x };//b vector but rotated by 90 degrees

        bool dot1 = hf_vec2f_dot(b_vec90, hf_vec2f_subtract(a.start, b.start)) > 0.f;
        bool dot2 = hf_vec2f_dot(b_vec90, hf_vec2f_subtract(a.end,   b.start)) > 0.f;

        if(dot1 == dot2) {
            return false;
        }
    }

    if(hit_point) {
        float angle = hf_vec2f_angle(a_vec);
        float rotation_sin = sinf(-angle);
        float rotation_cos = cosf(-angle);
        HF_Vec2f as_rot = hf_vec2f_rotate_cached(a.start, rotation_sin, rotation_cos);
        HF_Vec2f bs_rot = hf_vec2f_rotate_cached(b.start, rotation_sin, rotation_cos);
        HF_Vec2f bv_rot = hf_vec2f_rotate_cached(b_vec, rotation_sin, rotation_cos);

        float by_amp = fabsf(bv_rot.y);
        float lerp = by_amp > 0.f ? fabsf(as_rot.y - bs_rot.y) / by_amp : 0.5f;

        *hit_point = hf_vec2f_add(b.start, hf_vec2f_multiply(b_vec, lerp));
    }
    return true;
}

bool hf_intersection_triangles(HF_Triangle a, HF_Triangle b, HF_Vec2f* hit_point) {
    HF_Line edges_a[3];
    HF_Line edges_b[3];
    hf_triangle_get_edges(a, edges_a);
    hf_triangle_get_edges(b, edges_b);

    int hit_count = 0;
    HF_Vec2f mid_point = { 0.f, 0.f };
    for(int i = 0; i < 3; i++) {//check if any points of a are inside b and vice versa
        HF_Vec2f point_a = (&a.a)[i];
        if(hf_triangle_is_point_inside(b, point_a)) {
            hit_count++;
            mid_point = hf_vec2f_add(mid_point, point_a);
        }

        HF_Vec2f point_b = (&b.a)[i];
        if(hf_triangle_is_point_inside(a, point_b)) {
            hit_count++;
            mid_point = hf_vec2f_add(mid_point, point_b);
        }
    }
    for(int i = 0; i < 3; i++) {
        HF_Line edge_a = edges_a[i];

        for(int j = 0; j < 3; j++) {
            HF_Line edge_b = edges_b[j];
            HF_Vec2f line_hit_point;
            if(hf_intersection_lines(edge_a, edge_b, &line_hit_point)) {
                mid_point = hf_vec2f_add(mid_point, line_hit_point);
                hit_count++;
            }
        }
    }

    if(hit_count > 0 && hit_point) {
        *hit_point = hf_vec2f_divide(mid_point, (float)hit_count);
    }
    return hit_count > 0;// TODO: better hit point detection
}

bool hf_intersection_circles(HF_Circle a, HF_Circle b, HF_Vec2f* hit_point) {
    HF_Vec2f vec = hf_vec2f_subtract(a.position, b.position);
    bool retval = hf_vec2f_sqr_magnitude(vec) <= (a.radius + b.radius) * (a.radius + b.radius);
    if(hit_point) {
        *hit_point = hf_vec2f_divide(hf_vec2f_add(a.position, b.position), 2.f);
    }
    return retval;
}


bool hf_intersection_line_triangle(HF_Line line, HF_Triangle triangle, HF_Vec2f* hit_point) {
    HF_Line triangle_edges[3];
    hf_triangle_get_edges(triangle, triangle_edges);

    int hit_count = 0;
    HF_Vec2f mid_point = { 0.f, 0.f };
    for(int i = 0; i < 3; i++) {
        HF_Line edge = triangle_edges[i];
        HF_Vec2f line_hit;

        if(hf_intersection_lines(edge, line, &line_hit)) {
            mid_point = hf_vec2f_add(mid_point, line_hit);
            hit_count++;
        }
    }

    if(hit_count && hit_point) {
        *hit_point = hf_vec2f_divide(mid_point, (float)hit_count);
    }
    return hit_count > 0;
}


bool hf_intersection_line_circle(HF_Line line, HF_Circle circle, HF_Vec2f* hit_point) {
    HF_Vec2f closest_point = hf_line_closest_point(line, circle.position);
    HF_Vec2f vec = hf_vec2f_subtract(closest_point, circle.position);

    if(hit_point) {
        *hit_point = closest_point;
    }
    return hf_vec2f_sqr_magnitude(vec) <= circle.radius * circle.radius;
}


bool hf_intersection_triangle_circle(HF_Triangle triangle, HF_Circle circle, HF_Vec2f* hit_point) {
    HF_Line triangle_edges[3];
    hf_triangle_get_edges(triangle, triangle_edges);

    bool hit = false;
    HF_Vec2f best_point;
    float best_distance;
    for(int i = 0; i < 3; i++) {
        HF_Line edge = triangle_edges[i];
        HF_Vec2f line_hit;

        if(hf_intersection_line_circle(edge, circle, &line_hit)) {
            hit = true;
        }

        float new_dist = hf_vec2f_sqr_magnitude(hf_vec2f_subtract(line_hit, circle.position));
        if(i == 0 || new_dist < best_distance) {
            best_point = line_hit;
            best_distance = new_dist;
        }
    }

    if(hf_triangle_is_point_inside(triangle, circle.position)) {
        hit = true;
    }

    if(hit && hit_point) {
        *hit_point = best_point;
    }
    return hit;// TODO: better hit_point detection, maybe use closest point to circle center
}

