cmake_minimum_required(VERSION 3.18)

# Name of project
set(PROJ_NAME Project)
project(${PROJ_NAME})

# Specify project files: header files and source files
set(HDRS
    arrow.h
    background_controller_object.h
    background_sprite.h
    background_tile_object.h
    bullet_object.h
    charge_enemy_object.h
    collectible_game_object.h
    collision_handler.h
    config.h
    enemy_arm_object.h
    enemy_game_object.h
    explosion.h
    file_utils.h
    game.h
    game_object.h
    geometry.h
    health_bar.h
    health_bar_sprite.h
    helper.h
    melee_enemy_object.h
    particle_system.h
    particles.h
    player_game_object.h
    ranged_enemy_object.h
    saw_object.h
    shader.h
    sprite.h
    timer.h
    turret_object.h
    ui_object.h
    text_game_object.h
)
 
set(SRCS
    arrow.cpp
    background_controller_object.cpp
    background_sprite.cpp
    background_tile_object.cpp
    bullet_object.cpp
    charge_enemy_object.cpp
    collectible_game_object.cpp
    collision_handler.cpp
    enemy_arm_object.cpp
    enemy_game_object.cpp
    explosion.cpp
    file_utils.cpp
    game.cpp
    game_object.cpp
    health_bar.cpp
    health_bar_sprite.cpp
    helper.cpp
    main.cpp
    melee_enemy_object.cpp
    particle_fragment_shader.glsl
    particle_system.cpp
    particle_vertex_shader.glsl
    particles.cpp
    player_game_object.cpp
    ranged_enemy_object.cpp
    saw_object.cpp
    shader.cpp
    sprite.cpp
    sprite_vertex_shader.glsl
    sprite_fragment_shader.glsl
    timer.cpp
    turret_object.cpp
    text_game_object.cpp
    ui_fragment_shader.glsl
    ui_object.cpp
    ui_vertex_shader.glsl
    text_fragment_shader.glsl
)

# Add path name to configuration file
configure_file(path_config.h.in path_config.h)

# Add executable based on the source files
add_executable(${PROJ_NAME} ${HDRS} ${SRCS})

# Directories to include for header files, so that the compiler can find
# path_config.h
target_include_directories(${PROJ_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

# Require OpenGL library
set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(${PROJ_NAME} ${OPENGL_gl_LIBRARY})

# Other libraries needed
set(LIBRARY_PATH $ENV{COMP2501_LIBRARY_PATH} CACHE PATH "Folder with GLEW, GLFW, GLM, and SOIL libraries")
include_directories(${LIBRARY_PATH}/include)
if(NOT WIN32)
    find_library(GLEW_LIBRARY GLEW)
    find_library(GLFW_LIBRARY glfw)
    find_library(SOIL_LIBRARY SOIL)
elseif(WIN32)
    find_library(GLEW_LIBRARY glew32s HINTS ${LIBRARY_PATH}/lib)
    find_library(GLFW_LIBRARY glfw3 HINTS ${LIBRARY_PATH}/lib)
    find_library(SOIL_LIBRARY SOIL HINTS ${LIBRARY_PATH}/lib)
endif(NOT WIN32)
target_link_libraries(${PROJ_NAME} ${GLEW_LIBRARY})
target_link_libraries(${PROJ_NAME} ${GLFW_LIBRARY})
target_link_libraries(${PROJ_NAME} ${SOIL_LIBRARY})

# The rules here are specific to Windows Systems
if(WIN32)
    # Avoid ZERO_CHECK target in Visual Studio
    set(CMAKE_SUPPRESS_REGENERATION TRUE)
 
    # This will use the proper libraries in debug mode in Visual Studio
    set_target_properties(${PROJ_NAME} PROPERTIES DEBUG_POSTFIX _d)

    # Set the default project in VS
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJ_NAME})
endif(WIN32)
