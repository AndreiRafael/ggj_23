#ifndef HF_TRIANGLE_H
#define HF_TRIANGLE_H

#include <stdbool.h>
#include "hf_line.h"

typedef struct HF_Triangle_t {
    HF_Vec2f a;
    HF_Vec2f b;
    HF_Vec2f c;
} HF_Triangle;

HF_Vec2f hf_triangle_center(HF_Triangle triangle);
void     hf_triangle_get_edges(HF_Triangle triangle, HF_Line* edges_array);
bool     hf_triangle_is_point_inside(HF_Triangle triangle, HF_Vec2f point);
bool     hf_triangle_intersects(HF_Triangle a, HF_Triangle b, HF_Vec2f* hit_point);


#endif//HF_TRIANGLE_H

#ifdef HF_TRIANGLE_IMPLEMENTATION

void hf_triangle_get_edges(HF_Triangle triangle, HF_Line* edges_array) {
    edges_array[0] = (HF_Line){ triangle.a, triangle.b };
    edges_array[1] = (HF_Line){ triangle.b, triangle.c };
    edges_array[2] = (HF_Line){ triangle.c, triangle.a };
}

HF_Vec2f hf_triangle_center(HF_Triangle triangle) {
    return hf_vec2f_divide(hf_vec2f_add(triangle.a, hf_vec2f_add(triangle.b, triangle.c)), 3.f);
}

bool hf_triangle_is_point_inside(HF_Triangle triangle, HF_Vec2f point) {
    HF_Line tri_edges[3];
    hf_triangle_get_edges(triangle, tri_edges);
    HF_Vec2f center = hf_triangle_center(triangle);
    HF_Line line_to_center = { point, center };

    for(int i = 0; i < 3; i++) {
        if(hf_line_intersects(tri_edges[i], line_to_center, NULL)) {
            return false;
        }
    }
    return true;
}

bool hf_triangle_intersects(HF_Triangle a, HF_Triangle b, HF_Vec2f* hit_point) {
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
            if(hf_line_intersects(edge_a, edge_b, &line_hit_point)) {
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

#undef HF_TRIANGLE_IMPLEMENTATION
#endif//HF_TRIANGLE_IMPLEMENTATION
