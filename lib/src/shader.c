#include <stdlib.h>
#include <stdio.h>
#include "shader.h"
#include "glad/gl.h"
#include "util.h"

typedef struct Shader_t
{
  GLuint shader;
} Shader_t;

Shader_t *Shader_Init(void)
{
  return malloc(sizeof(Shader_t));
}

void Shader_Load(Shader_t *shader, const char *vs, const char *fs)
{
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vs, NULL);
  glCompileShader(vertexShader);

  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    LOG("Failed to compile vertex shader: %s\n", infoLog);
  }

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fs, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    LOG("Failed to compile fragment shader: %s: \n", infoLog);
  }

  shader->shader = glCreateProgram();
  glAttachShader(shader->shader, vertexShader);
  glAttachShader(shader->shader, fragmentShader);
  glLinkProgram(shader->shader);
}

void Shader_SetMatrix4(Shader_t *shader, const char *name, const mat4x4 *mat)
{
  glUniformMatrix4fv(
    glGetUniformLocation(shader->shader, name),
    1,
    GL_FALSE,
    (const GLfloat*)mat);
}

void Shader_SetVec2(Shader_t *shader, const char *name, const vec2 vec)
{
  glUniform2f(glGetUniformLocation(shader->shader, name), vec[0], vec[1]);
}

void Shader_SetVec3(Shader_t *shader, const char *name, const vec3 vec)
{
  glUniform3f(glGetUniformLocation(shader->shader, name), vec[0], vec[1], vec[2]);
}

void Shader_SetVec4(Shader_t *shader, const char *name, const vec4 vec)
{
  glUniform4f(glGetUniformLocation(shader->shader, name), vec[0], vec[1], vec[2], vec[3]);
}

void Shader_SetFloat(Shader_t *shader, const char *name, const f32 value)
{
  glUniform1f(glGetUniformLocation(shader->shader, name), value);
}

void Shader_SetUint(Shader_t *shader, const char *name, const u32 value)
{
  glUniform1i(glGetUniformLocation(shader->shader, name), value);
}

void Shader_Use(Shader_t * shader)
{
  glUseProgram(shader->shader);
}

void Shader_Delete(Shader_t *shader)
{
  glDeleteProgram(shader->shader);
  free(shader);
}