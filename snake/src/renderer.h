#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "linmath.h"

typedef struct Renderer_t Renderer_t;
typedef struct Shader_t Shader_t;

Renderer_t *Renderer_Init(float *vertices, int size_vertices);
Renderer_t *Renderer_Init_With_Shader(float *vertices, int size_vertices, Shader_t *shader);
void Renderer_RenderObject(Renderer_t *renderer, vec2 position, vec2 size, vec4 color);
void Renderer_Delete(Renderer_t *renderer);

#endif /* _RENDERER_H_ */