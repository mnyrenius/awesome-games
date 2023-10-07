cmake_minimum_required(VERSION 3.11)

include(../lib/cmake/shader.cmake)
include(FetchContent)
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG master)
FetchContent_MakeAvailable(glfw)

execute_process(
  COMMAND git rev-parse --short HEAD
  OUTPUT_STRIP_TRAILING_WHITESPACE
  OUTPUT_VARIABLE ASNAKE_BUILD
)

configure_file(
  src/build_info.h.in
  gen/build_info.h
  @ONLY
)

add_compile_options(
  -Wall -Werror
  $<$<CONFIG:RELEASE>:-O3>
  $<$<CONFIG:DEBUG>:-O0>
)

add_executable(asnake
  src/game.c
  src/highscores.c
  src/hud.c
  src/level.c
  src/menu.c
  src/player.c
  src/asnake.c
)

target_include_directories(
  asnake
  PRIVATE
  src
  ${CMAKE_CURRENT_BINARY_DIR}/gen
)

target_link_libraries(asnake
  aglib
  glfw
  m
)

add_shader(asnake title.vs)
add_shader(asnake title.fs)
add_shader(asnake wall.fs)
add_shader(asnake food.fs)
add_shader(asnake player.fs)
