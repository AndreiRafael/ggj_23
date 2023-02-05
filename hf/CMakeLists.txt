set(hf_math_sources
    hf_intersection.c
    hf_line.c
    hf_transform.c
    hf_triangle.c
    hf_vec.c
)
list(TRANSFORM hf_math_sources PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/src/)
add_library(hf_math ${hf_math_sources})
target_compile_options(hf_math PRIVATE -Wstrict-prototypes -Wconversion -Wall -Wextra -Wpedantic -pedantic -Werror)
