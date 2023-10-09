#pragma once

#include "linmath.h"

typedef struct SpriteRenderer_t SpriteRenderer_t;
typedef struct Shader_t Shader_t;
typedef struct Texture_t Texture_t;

SpriteRenderer_t *SpriteRenderer_Init(void);
void SpriteRenderer_RenderObject(SpriteRenderer_t *renderer, Texture_t *texture, vec2 position, vec2 size, vec4 color);
void SpriteRenderer_Delete(SpriteRenderer_t *renderer);