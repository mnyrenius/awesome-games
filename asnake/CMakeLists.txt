cmake_minimum_required(VERSION 3.11)

include(FetchContent)
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG master)
FetchContent_MakeAvailable(glfw)

function(add_shader SHADER)

  add_custom_target(shader_${SHADER}
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/gen/shaders
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/${SHADER} | xxd -i > ${CMAKE_CURRENT_BINARY_DIR}/gen/shaders/${SHADER}.data
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/${SHADER}
  )
  add_dependencies(asnake shader_${SHADER})
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/${SHADER})
endfunction()

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
  glad/src/gl.c
  src/collision.c
  src/game.c
  src/highscores.c
  src/hud.c
  src/level.c
  src/menu.c
  src/player.c
  src/asnake.c
  src/shader.c
  src/renderer.c
  src/text_renderer.c
  src/util.c
)

target_include_directories(
  asnake
  PRIVATE
  src/
  glad/include/
  ${CMAKE_CURRENT_BINARY_DIR}/gen/
)

target_link_libraries(asnake
  glfw
  m
)

add_shader(generic_object.vs)
add_shader(generic_object.fs)
add_shader(text.vs)
add_shader(text.fs)
add_shader(title.vs)
add_shader(title.fs)
add_shader(wall.fs)
add_shader(food.fs)
add_shader(player.fs)
