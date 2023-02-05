#include <stdbool.h>
#include <stdlib.h>
#include "../include/hf_triangle.h"
#include "../include/hf_intersection.h"

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
        if(hf_intersection_lines(tri_edges[i], line_to_center, NULL)) {
            return false;
        }
    }
    return true;
}
