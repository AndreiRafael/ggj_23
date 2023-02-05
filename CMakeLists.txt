cmake_minimum_required(VERSION 3.2)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(MY_PROJECT_NAME trepadeira)
project(${MY_PROJECT_NAME} C)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/debug)
else()
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/release)
endif()

set(main_sources
    main.c
	vine.c
	world.c
)
list(TRANSFORM main_sources PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/src/)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_executable(${MY_PROJECT_NAME} ${main_sources})
else()
	if(WIN32)#hide console window on release
		add_executable(${MY_PROJECT_NAME} WIN32 ${main_sources})
	else()
		add_executable(${MY_PROJECT_NAME} ${main_sources})
	endif()
endif()

# Make compiler scream out every possible warning
target_compile_options(${MY_PROJECT_NAME} PRIVATE -Wstrict-prototypes -Wconversion -Wall -Wextra -Wpedantic -pedantic -Werror)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/hf)
include_directories(${MY_PROJECT_NAME} ${CMAKE_SOURCE_DIR}/include)
include_directories(${MY_PROJECT_NAME} ${CMAKE_SOURCE_DIR}/hf/include)

target_link_directories(${MY_PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/sdl)
target_link_libraries(${MY_PROJECT_NAME} PUBLIC SDL2main SDL2 hf_math)

target_link_directories(${MY_PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/sdl_mixer)
target_link_libraries(${MY_PROJECT_NAME} PUBLIC SDL2_mixer)

target_link_directories(${MY_PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/sdl_ttf)
target_link_libraries(${MY_PROJECT_NAME} PUBLIC SDL2_ttf)

#copy sdl dlls to executable path
file(COPY ${CMAKE_SOURCE_DIR}/lib/sdl/SDL2.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY ${CMAKE_SOURCE_DIR}/lib/sdl_mixer/SDL2_mixer.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY ${CMAKE_SOURCE_DIR}/lib/sdl_ttf/SDL2_ttf.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

#copy all resources
file(REMOVE_RECURSE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
