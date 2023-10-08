cmake_minimum_required(VERSION 3.11)
include(cmake/shader.cmake)

add_library(aglib OBJECT
  3pp/glad/src/gl.c
  src/collision.c
  src/renderer.c
  src/shader.c
  src/text_renderer.c
  src/util.c
)

target_include_directories(aglib PUBLIC
  .
  3pp
  3pp/glad/include
  ${CMAKE_CURRENT_BINARY_DIR}/gen
)

add_shader(aglib generic_object.vs)
add_shader(aglib generic_object.fs)
add_shader(aglib text.vs)
add_shader(aglib text.fs)