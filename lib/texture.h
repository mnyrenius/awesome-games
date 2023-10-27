#pragma once

#include "types.h"

typedef struct Texture_t Texture_t;

Texture_t *Texture_Init(u8 *texture_data, u32 len, u32 num_sprites);
void Texture_Use(Texture_t *texture);
u32 Texture_GetNumSprites(Texture_t *texture);
u32 Texture_GetWidth(Texture_t *texture);
u32 Texture_GetHeight(Texture_t *texture);
void Texture_Delete(Texture_t *texture);