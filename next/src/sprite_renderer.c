#include <stdlib.h>
#include "glad/gl.h"
#include "sprite_renderer.h"
#include "shader.h"
#include "util.h"
#include "types.h"
#include "global.h"
#include "texture.h"

typedef struct SpriteRenderer_t
{
  GLuint vao;
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
    ,0
  };

  const char fs[] =
  {
    #include "shaders/sprite.fs.data"
    ,0
  };
  Shader_Load(renderer->shader, vs, fs);

  float vertices[] = {
      0.0f, 1.0f, 0.0f, 1.0f,
      1.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 0.0f,

      0.0f, 1.0f, 0.0f, 1.0f,
      1.0f, 1.0f, 1.0f, 1.0f,
      1.0f, 0.0f, 1.0f, 0.0f};

  GLuint vbo;
  glGenVertexArrays(1, &renderer->vao);
  glGenBuffers(1, &vbo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(renderer->vao);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  CheckGlErrors();

  mat4x4 projection;
  mat4x4_ortho(projection, 0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
  Shader_Use(renderer->shader);
  Shader_SetMatrix4(renderer->shader, "projection", &projection);

  return renderer;
}

void SpriteRenderer_RenderObject(SpriteRenderer_t *renderer, Texture_t *texture, vec2 position, vec2 size, vec4 color)
{
  mat4x4 model;
  mat4x4_identity(model);
  mat4x4_translate(model, position[0], position[1], 0.0f);
  mat4x4 scale;
  mat4x4_identity(scale);
  model[0][0] *= size[0];
  model[1][1] *= size[1];

  Shader_Use(renderer->shader);
  Shader_SetMatrix4(renderer->shader, "model", &model);
  Shader_SetVec4(renderer->shader, "spriteColor", color);
  Shader_SetFloat(renderer->shader, "time", Global_Time.now);
  Shader_SetUint(renderer->shader, "num_sprites", Texture_GetNumSprites(texture));

  Texture_Use(texture);
  CheckGlErrors();

  glBindVertexArray(renderer->vao);
  CheckGlErrors();

  glDrawArrays(GL_TRIANGLES, 0, 6);
  CheckGlErrors();
  glBindVertexArray(0);
  CheckGlErrors();
}

void SpriteRenderer_Delete(SpriteRenderer_t *renderer)
{
  Shader_Delete(renderer->shader);
  free(renderer);
}