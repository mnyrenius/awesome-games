#ifndef _TEXT_RENDERER_H_
#define _TEXT_RENDERER_H_

#include "linmath.h"

typedef struct TextRenderer_t TextRenderer_t;
typedef struct Shader_t Shader_t;

TextRenderer_t *TextRenderer_Init(void);
TextRenderer_t *TextRenderer_Init_With_Shader(Shader_t *shader);
void TextRenderer_RenderString(TextRenderer_t *renderer, const char *str, vec2 position, float size);

#endif /* TEXT_RENDERER_H_ */