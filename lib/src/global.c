#include "global.h"
#include <GLFW/glfw3.h>

Global_Time_t Global_Time = { 0 };

void Global_Time_Update(void)
{
  f32 prev = Global_Time.now;
  Global_Time.now = glfwGetTime();
  Global_Time.dt = Global_Time.now - prev;
}
