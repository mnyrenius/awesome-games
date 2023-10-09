#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include "game.h"
#include "util.h"
#include "global.h"

#define WIDTH 800
#define HEIGHT 600


static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  Game_t *game = glfwGetWindowUserPointer(window);
  Game_UpdateKeys(game, key, action);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
  GLFWwindow* window;
  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  window = glfwCreateWindow(WIDTH, HEIGHT, "Next", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, key_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glfwMakeContextCurrent(window);
  gladLoadGL(glfwGetProcAddress);
  glfwSwapInterval(1);

  glViewport(0, 0, WIDTH, HEIGHT);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Game_t * game = Game_Init(WIDTH, HEIGHT);
  glfwSetWindowUserPointer(window, game);

  float delta_time = 0.0f;
  float fps_time = 0.0f;
  int fps_counter = 0;

  while (!glfwWindowShouldClose(window))
  {
    glClear(GL_COLOR_BUFFER_BIT);
    Global_Time_Update();
    delta_time = Global_Time.dt;
    glfwPollEvents();

    Game_Update(game, delta_time);
    if (Game_IsQuit(game))
    {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    glfwSwapBuffers(window);

    fps_time += delta_time;
    fps_counter++;
    if (fps_time > 1.0f)
    {
      LOG("fps: %d\n", fps_counter);
      fps_counter = 0;
      fps_time = 0.0f;
    }
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  exit(EXIT_SUCCESS);
}