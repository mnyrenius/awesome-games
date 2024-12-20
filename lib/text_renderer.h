#pragma once

#include "linmath.h"

typedef struct TextRenderer_t TextRenderer_t;
typedef struct Shader_t Shader_t;

TextRenderer_t *TextRenderer_Init(void);
TextRenderer_t *TextRenderer_Init_With_Shader(Shader_t *shader);
void TextRenderer_RenderString(TextRenderer_t *renderer, const char *str, vec2 position, float size);
void TextRenderer_RenderString_WithColor(TextRenderer_t *renderer, const char *str, vec2 position, float size, vec3 color);
void TextRenderer_Delete(TextRenderer_t *renderer);
