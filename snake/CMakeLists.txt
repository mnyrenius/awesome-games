cmake_minimum_required(VERSION 3.11)
project(snake VERSION 0.0.1 DESCRIPTION "Snake")

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(snake
  glad/src/gl.c
  src/collision.c
  src/game.c
  src/hud.c
  src/level.c
  src/player.c
  src/snake.c
  src/shader.c
  src/renderer.c
  src/text_renderer.c
  src/util.c
)

target_include_directories(
  snake
  PRIVATE
  src/
  glad/include/
)

target_link_libraries(snake
  OpenGL::GL
  glfw
  dl
)

