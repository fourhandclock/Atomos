cmake_minimum_required(VERSION 3.14)
project(Atomos)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

add_executable(Atomos main.cpp glad.c renderer.cpp physics.cpp )

target_include_directories(Atomos PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(Atomos PRIVATE ${CMAKE_SOURCE_DIR}/eigen)

# Directly link the absolute path to the GLFW library file
target_link_libraries(Atomos PRIVATE 
    "${CMAKE_SOURCE_DIR}/lib/libglfw3.a" 
    opengl32 
    gdi32
)

add_custom_command(TARGET Atomos POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/shader.comp"
    $<TARGET_FILE_DIR:Atomos>
)

# add_custom_command(TARGET Atomos POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#     "${CMAKE_CURRENT_SOURCE_DIR}/shader.frag"
#     $<TARGET_FILE_DIR:Atomos>
# )