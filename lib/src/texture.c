#include <stdlib.h>
#include "texture.h"
#include "glad/gl.h"
#include "util.h"
#include "stb_image.h"

typedef struct Texture_t
{
  GLuint id;
  u32 num_sprites;
  u32 width;
  u32 height;
} Texture_t;

Texture_t *Texture_Init(u8 *texture_data, u32 len, u32 num_sprites)
{
  Texture_t *texture = malloc(sizeof(Texture_t));
  texture->num_sprites = num_sprites;

  int no_channels;
  u8 *data = stbi_load_from_memory(texture_data, len, (int*)&texture->width, (int*)&texture->height, &no_channels, 0);
  (void) no_channels;

  glGenTextures(1, &texture->id);
  glBindTexture(GL_TEXTURE_2D, texture->id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  free(data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);

  CheckGlErrors();

  return texture;
}

void Texture_Use(Texture_t *texture)
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture->id);
}

u32 Texture_GetNumSprites(Texture_t *texture)
{
  return texture->num_sprites;
}

u32 Texture_GetWidth(Texture_t *texture)
{
  return texture->width;
}

u32 Texture_GetHeight(Texture_t *texture)
{
  return texture->height;
}

void Texture_Delete(Texture_t *texture)
{
  glDeleteTextures(1, &texture->id);
}