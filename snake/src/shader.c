#include <stdlib.h>
#include <stdio.h>
#include "shader.h"
#include "glad/gl.h"

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
    printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED: %s\n", infoLog);
  }

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fs, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED: %s\n", infoLog);
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


void Shader_SetVec3(Shader_t *shader, const char *name, const vec3 vec)
{
  glUniform3f(glGetUniformLocation(shader->shader, name), vec[0], vec[1], vec[2]);
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