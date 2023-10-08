cmake_minimum_required(VERSION 3.11)
project(awesome-games)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  include(FetchContent)
  FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG master)
  FetchContent_MakeAvailable(glfw)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  find_package(glfw3 3.3 REQUIRED)
endif()

add_compile_options(
  -Wall -Werror
  $<$<CONFIG:RELEASE>:-O3>
  $<$<CONFIG:DEBUG>:-O0>
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(lib)
add_subdirectory(asnake)
add_subdirectory(next)