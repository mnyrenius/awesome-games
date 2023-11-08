#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "glad/gl.h"
#include "sprite_renderer.h"
#include "shader.h"
#include "util.h"
#include "types.h"
#include "global.h"
#include "texture.h"

#define MAX_VERTICES 1024

typedef struct SpriteRenderer_Vertex_t
{
  vec2 xy;
  vec2 uv;
} SpriteRenderer_Vertex_t;

typedef struct SpriteRenderer_t
{
  GLuint vao;
  GLuint vbo;
  GLuint textureId;
  Shader_t *shader;
} SpriteRenderer_t;

SpriteRenderer_t *SpriteRenderer_Init(void)
{
  SpriteRenderer_t *renderer = malloc(sizeof(SpriteRenderer_t));
  renderer->shader = Shader_Init();
  const char vs[] =
      {
#include "shaders/sprite.vs.data"
          , 0};

  const char fs[] =
      {
#include "shaders/sprite.fs.data"
          , 0};
  Shader_Load(renderer->shader, vs, fs);

  glGenVertexArrays(1, &renderer->vao);
  glGenBuffers(1, &renderer->vbo);

  glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
  glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(SpriteRenderer_Vertex_t), 0, GL_DYNAMIC_DRAW);

  glBindVertexArray(renderer->vao);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteRenderer_Vertex_t), (void *)offsetof(SpriteRenderer_Vertex_t, xy));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteRenderer_Vertex_t), (void *)offsetof(SpriteRenderer_Vertex_t, uv));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  CheckGlErrors();

  mat4x4 projection;
  mat4x4_ortho(projection, 0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
  Shader_Use(renderer->shader);
  Shader_SetMatrix4(renderer->shader, "projection", &projection);

  return renderer;
}

void SpriteRenderer_RenderObject(SpriteRenderer_t *renderer, Texture_t *texture, vec2 position, vec2 size, vec2 uv, bool flip)
{
  SpriteRenderer_RenderObject_With_Color(renderer, texture, position, size, uv, (vec4){1.0f, 1.0f, 1.0f, 1.0f}, flip);
}

void SpriteRenderer_RenderObject_With_Color(SpriteRenderer_t *renderer, Texture_t *texture, vec2 position, vec2 size, vec2 uv, vec4 color, bool flip)
{
  f32 tw = (f32)Texture_GetWidth(texture) / Texture_GetNumSprites(texture);
  f32 th = (f32)Texture_GetHeight(texture);

  SpriteRenderer_Vertex_t vertices[] =
      {
          {{position[0], position[1] + size[1]}, {uv[0] / tw, (uv[1] + size[1]) / th}},
          {{position[0] + size[0], position[1]}, {(uv[0] + size[0]) / tw, uv[1] / th}},
          {{position[0], position[1]}, {uv[0] / tw, uv[1] / th}},
          {{position[0], position[1] + size[1]}, {uv[0] / tw, (uv[1] + size[1]) / th}},
          {{position[0] + size[0], position[1] + size[1]}, {(uv[0] + size[0]) / tw, (uv[1] + size[1]) / th}},
          {{position[0] + size[0], position[1]}, {(uv[0] + size[0]) / tw, uv[1] / th}},
      };

  Shader_Use(renderer->shader);
  Shader_SetUint(renderer->shader, "flip_x", flip);
  Shader_SetFloat(renderer->shader, "time", Global_Time.now);
  Shader_SetUint(renderer->shader, "num_sprites", Texture_GetNumSprites(texture));
  Shader_SetVec4(renderer->shader, "sprite_color", color);

  Texture_Use(texture);
  CheckGlErrors();
  glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, 6 * sizeof(SpriteRenderer_Vertex_t), vertices);
  glBindVertexArray(renderer->vao);
  CheckGlErrors();

  glDrawArrays(GL_TRIANGLES, 0, 6);
  CheckGlErrors();
  glBindVertexArray(0);
  CheckGlErrors();
}

void SpriteRenderer_UpdateOrtho(SpriteRenderer_t *renderer, vec2 view)
{
  mat4x4 projection;
  mat4x4_ortho(projection, view[0], view[0] + 800.0f, view[1] + 600.0f, view[1], -1.0f, 1.0f);
  Shader_Use(renderer->shader);
  Shader_SetMatrix4(renderer->shader, "projection", &projection);
}

void SpriteRenderer_Delete(SpriteRenderer_t *renderer)
{
  Shader_Delete(renderer->shader);
  glDeleteBuffers(1, &renderer->vbo);
  glDeleteVertexArrays(1, &renderer->vao);
  free(renderer);
}