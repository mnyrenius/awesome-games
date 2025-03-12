cmake_minimum_required(VERSION 3.11)

include(../lib/cmake/shader.cmake)

execute_process(
  COMMAND git rev-parse --short HEAD
  OUTPUT_STRIP_TRAILING_WHITESPACE
  OUTPUT_VARIABLE HOPPSPEL_BUILD
)

configure_file(
  src/build_info.h.in
  gen/build_info.h
  @ONLY
)

add_executable(hoppspel
  src/game.c
  src/highscores.c
  src/hud.c
  src/level.c
  src/menu.c
  src/hoppspel.c
  src/player.c
  resources/sprites.c
  src/sprite_renderer.c
  src/level_selection.c
)

target_include_directories(
  hoppspel
  PRIVATE
  src
  resources
  ${CMAKE_CURRENT_BINARY_DIR}/gen
)

target_link_libraries(hoppspel
  aglib
  glfw
  m
  ${CMAKE_DL_LIBS}
)

add_shader(hoppspel title.vs)
add_shader(hoppspel title.fs)