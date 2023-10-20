#pragma once

#include "linmath.h"
#include "types.h"

typedef struct Shader_t Shader_t;

Shader_t * Shader_Init(void);
void Shader_Load(Shader_t *shader, const char *vs, const char *fs);
void Shader_SetMatrix4(Shader_t *shader, const char *name, const mat4x4 *mat);
void Shader_SetVec2(Shader_t *shader, const char *name, const vec2 vec);
void Shader_SetVec3(Shader_t *shader, const char *name, const vec3 vec);
void Shader_SetVec4(Shader_t *shader, const char *name, const vec4 vec);
void Shader_SetFloat(Shader_t *shader, const char *name, const f32 value);
void Shader_SetUint(Shader_t *shader, const char *name, const u32 value);
void Shader_Use(Shader_t *shader);
void Shader_Delete(Shader_t *shader);
