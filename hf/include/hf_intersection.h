#ifndef HF_INTERSECTION_H
#define HF_INTERSECTION_H

#include "hf_vec.h"
#include "hf_line.h"
#include "hf_triangle.h"
#include "hf_circle.h"
#include <stdbool.h>

bool hf_intersection_lines(HF_Line a, HF_Line b, HF_Vec2f* hit_point);
bool hf_intersection_triangles(HF_Triangle a, HF_Triangle b, HF_Vec2f* hit_point);
bool hf_intersection_circles(HF_Circle a, HF_Circle b, HF_Vec2f* hit_point);

bool hf_intersection_line_triangle(HF_Line line, HF_Triangle triangle, HF_Vec2f* hit_point);
#define hf_intersection_triangle_line(triangle, line, hit_point) hf_intersection_line_triangle(line, triangle, hit_point)

bool hf_intersection_line_circle(HF_Line line, HF_Circle circle, HF_Vec2f* hit_point);
#define hf_intersection_circle_line(circle, line, hit_point) hf_intersection_line_circle(line, circle, hit_point)

bool hf_intersection_triangle_circle(HF_Triangle triangle, HF_Circle circle, HF_Vec2f* hit_point);
#define hf_intersection_circle_triangle(circle, triangle, hit_point) hf_intersection_triangle_circle(triangle, circle, hit_point)

#endif//HF_INTERSECTION_H
