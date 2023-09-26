#include "util.h"

#include <stdio.h>
#include "glad/gl.h"

void check_gl_errors(const char *file, int line)
{
  GLenum error_code;
  while ((error_code = glGetError()) != GL_NO_ERROR)
  {
    printf("[%s:%d]: Error: %d\n", file, line, error_code);
  }
}