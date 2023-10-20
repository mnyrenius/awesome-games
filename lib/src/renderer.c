#include <stdlib.h>
#include <stdio.h>
#include "glad/gl.h"
#include "renderer.h"
#include "shader.h"
#include "util.h"

typedef struct Renderer_t
{
  GLuint vao;
  Shader_t *shader;
} Renderer_t;

Renderer_t *Renderer_Init(float *vertices, int size_vertices)
{
  Shader_t *shader = Shader_Init();
  const char vs[] =
  {
    #include "shaders/generic_object.vs.data"
    ,0
  };

  const char fs[] =
  {
    #include "shaders/generic_object.fs.data"
    ,0
  };
  Shader_Load(shader, vs, fs);
  return Renderer_Init_With_Shader(vertices, size_vertices, shader);
}

Renderer_t *Renderer_Init_With_Shader(float *vertices, int size_vertices, Shader_t *shader)
{
  Renderer_t *renderer = malloc(sizeof(Renderer_t));
  renderer->shader = shader;

  GLuint vbo;
  glGenVertexArrays(1, &renderer->vao);
  glGenBuffers(1, &vbo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, size_vertices, vertices, GL_STATIC_DRAW);

  glBindVertexArray(renderer->vao);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  CheckGlErrors();

  mat4x4 projection;
  mat4x4_ortho(projection, 0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
  Shader_Use(renderer->shader);
  Shader_SetMatrix4(renderer->shader, "projection", &projection);

  return renderer;
}

void Renderer_RenderObject(Renderer_t *renderer, vec2 position, vec2 size, vec4 color)
{
  Shader_Use(renderer->shader);
  glBindVertexArray(renderer->vao);
  CheckGlErrors();
  mat4x4 model;
  mat4x4_identity(model);
  mat4x4_translate(model, position[0], position[1], 0.0f);
  mat4x4 scale;
  mat4x4_identity(scale);
  model[0][0] *= size[0];
  model[1][1] *= size[1];
  Shader_SetMatrix4(renderer->shader, "model", &model);
  Shader_SetVec4(renderer->shader, "color", color);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glBindVertexArray(0);
  CheckGlErrors();
}

void Renderer_UpdateOrtho(Renderer_t *renderer, vec2 view)
{
  mat4x4 projection;
  mat4x4_ortho(projection, view[0], view[0] + 800.0f, view[1] + 600.0f, view[1], -1.0f, 1.0f);
  Shader_Use(renderer->shader);
  Shader_SetMatrix4(renderer->shader, "projection", &projection);
}

void Renderer_Delete(Renderer_t *renderer)
{
  Shader_Delete(renderer->shader);
  glDeleteVertexArrays(1, &renderer->vao);
  free(renderer);
}