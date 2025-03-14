#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "glad/gl.h"
#include "text_renderer.h"
#include "shader.h"
#include "util.h"
#include "font8x8_basic.h"

typedef struct TextRenderer_t
{
  GLuint vao;
  GLuint vbo;
  GLuint textureId;
  Shader_t *shader;
} TextRenderer_t;

void render_character(unsigned char *buffer, char c, int x_pos, int line_width)
{
  int x, y;
  int set;
  char *bitmap = font8x8_basic[(int)c];
  buffer += x_pos;
  for (x = 0; x < 8; x++)
  {
    for (y = 0; y < 8; y++)
    {
      set = bitmap[x] & 1 << y;
      *(buffer++) = set ? 0xff : 0x00;
    }
    buffer += line_width - 8;
  }
}

void render_string(unsigned char *buffer, const char *str)
{
  int x_pos = 0;
  for (int i = 0; i < strlen(str); ++i)
  {
    render_character(buffer, str[i], x_pos, strlen(str) * 8);
    x_pos += 8;
  }
}

TextRenderer_t *TextRenderer_Init(void)
{
  Shader_t *shader = Shader_Init();
  const char vs[] =
  {
    #include "shaders/text.vs.data"
    ,0
  };

  const char fs[] =
  {
    #include "shaders/text.fs.data"
    ,0
  };
  Shader_Load(shader, vs, fs);
  return TextRenderer_Init_With_Shader(shader);
}

TextRenderer_t *TextRenderer_Init_With_Shader(Shader_t *shader)
{
  TextRenderer_t *renderer = malloc(sizeof(TextRenderer_t));
  renderer->shader = shader;

  float vertices[] = {
      0.0f, 1.0f, 0.0f, 1.0f,
      1.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 0.0f,

      0.0f, 1.0f, 0.0f, 1.0f,
      1.0f, 1.0f, 1.0f, 1.0f,
      1.0f, 0.0f, 1.0f, 0.0f};

  glGenVertexArrays(1, &renderer->vao);
  glGenBuffers(1, &renderer->vbo);

  glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
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
  vec3 color = { 1.0f, 1.0f, 1.0f };
  Shader_SetVec3(renderer->shader, "color", color);

  glGenTextures(1, &renderer->textureId);
  glBindTexture(GL_TEXTURE_2D, renderer->textureId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);
  CheckGlErrors();

return renderer;
}

void TextRenderer_RenderString_WithColor(TextRenderer_t *renderer, const char *str, vec2 position, float size, vec3 color)
{
  int width = strlen(str) * 8;
  int height = 8;

  unsigned char *buffer = malloc(width * height);

  render_string(buffer, str);

  mat4x4 model;
  mat4x4_identity(model);
  mat4x4_translate(model, position[0], position[1], 0.0f);
  mat4x4 scale;
  mat4x4_identity(scale);
  model[0][0] *= width * size;
  model[1][1] *= height * size;

  Shader_Use(renderer->shader);
  Shader_SetMatrix4(renderer->shader, "model", &model);
  Shader_SetVec3(renderer->shader, "color", color);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, renderer->textureId);
  CheckGlErrors();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, buffer);
  CheckGlErrors();

  glBindVertexArray(renderer->vao);
  CheckGlErrors();

  glDrawArrays(GL_TRIANGLES, 0, 6);
  CheckGlErrors();
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
  CheckGlErrors();
  free(buffer);
}

void TextRenderer_RenderString(TextRenderer_t *renderer, const char *str, vec2 position, float size)
{
  vec3 color = { 1.0f, 1.0f, 1.0f };
  TextRenderer_RenderString_WithColor(renderer, str, position, size, color);
}

void TextRenderer_Delete(TextRenderer_t *renderer)
{
  Shader_Delete(renderer->shader);
  glDeleteBuffers(1, &renderer->vbo);
  glDeleteVertexArrays(1, &renderer->vao);
}
