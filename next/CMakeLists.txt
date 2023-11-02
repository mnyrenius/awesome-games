cmake_minimum_required(VERSION 3.11)

include(../lib/cmake/shader.cmake)

add_executable(next
  src/game.c
  src/hud.c
  src/level.c
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
