cmake_minimum_required(VERSION 3.11)
project(asnake VERSION 0.0.1 DESCRIPTION "Action Snake")

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)

function(add_shader SHADER)

  add_custom_target(shader_${SHADER}
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/gen/shaders
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/${SHADER} | xxd -i > ${CMAKE_CURRENT_BINARY_DIR}/gen/shaders/${SHADER}.data
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/${SHADER}
  )
  add_dependencies(${CMAKE_PROJECT_NAME} shader_${SHADER})
  #set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/${SHADER})
endfunction()

add_executable(${CMAKE_PROJECT_NAME}
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
  ${CMAKE_PROJECT_NAME}
  PRIVATE
  src/
  glad/include/
  ${CMAKE_CURRENT_BINARY_DIR}/gen/
)

target_link_libraries(${CMAKE_PROJECT_NAME}
  OpenGL::GL
  glfw
  dl
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
