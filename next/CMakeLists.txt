cmake_minimum_required(VERSION 3.11)

include(../lib/cmake/shader.cmake)

execute_process(
  COMMAND git rev-parse --short HEAD
  OUTPUT_STRIP_TRAILING_WHITESPACE
  OUTPUT_VARIABLE NEXT_BUILD
)

configure_file(
  src/build_info.h.in
  gen/build_info.h
  @ONLY
)

add_executable(next
  src/game.c
  src/hud.c
  src/level.c
  src/menu.c
  src/next.c
  src/player.c
  resources/sprites.c
  src/sprite_renderer.c
)

target_include_directories(
  next
  PRIVATE
  src
  resources
  ${CMAKE_CURRENT_BINARY_DIR}/gen
)

target_link_libraries(next
  aglib
  glfw
  m
  ${CMAKE_DL_LIBS}
)
