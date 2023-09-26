#ifndef _SHADER_H_
#define _SHADER_H_

#include "linmath.h"

typedef struct Shader_t Shader_t;

Shader_t * Shader_Init(void);
void Shader_Load(Shader_t *shader, const char *vs, const char *fs);
void Shader_SetMatrix4(Shader_t *shader, const char *name, const mat4x4 *mat);
void Shader_SetVec3(Shader_t *shader, const char *name, const vec3 vec);
void Shader_Use(Shader_t *shader);
void Shader_Delete(Shader_t *shader);

#endif /* _SHADER_H_ */