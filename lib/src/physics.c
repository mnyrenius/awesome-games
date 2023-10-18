#include <stdlib.h>
#include <stdbool.h>
#include "physics.h"
#include "types.h"
#include "entity.h"
#include "util.h"

typedef struct Entry_t
{
  Entity_t *entity;
  Physics_OnCollision on_collision;
} Entry_t;

typedef struct Physics_t
{
  Entry_t *entities;
  u32 num_entities;
} Physics_t;

static bool is_collision_aabb(vec2 a, vec2 a_size, vec2 b, vec2 b_size)
{
  bool result = false;

  if (((a[0] + a_size[0]) >= b[0]) && (a[0] <= (b[0] + b_size[0])) &&
         ((a[1] + a_size[1]) >= b[1]) && (a[1] <= (b[1] + b_size[1])))
         {
          result = true;
         }

  return result;
}

Physics_t *Physics_Init(void)
{
  Physics_t *physics = malloc(sizeof(Physics_t));
  physics->entities = malloc(sizeof(Entry_t) * 16);
  physics->num_entities = 0;
  return physics;
}

void Physics_AddEntity(Physics_t *physics, Entity_t *entity, Physics_OnCollision on_collision)
{
  u32 idx = physics->num_entities++;
  physics->entities[idx].entity = entity;
  physics->entities[idx].on_collision = on_collision;
}

void Physics_Update(Physics_t *physics, f32 dt)
{
  for (u32 i = 0; i < physics->num_entities; ++i)
  {
    Entity_t *entity = physics->entities[i].entity;
    entity->velocity[0] += entity->force[0] * dt;
    entity->velocity[1] += entity->force[1] * dt;
    entity->position[0] += entity->velocity[0] * dt;
    entity->position[1] += entity->velocity[1] * dt;
  }

  for (u32 i = 0; i < physics->num_entities; ++i)
    for (u32 j = 0; j < physics->num_entities; ++j)
    {
      Entity_t *a = physics->entities[i].entity;
      Entity_t *b = physics->entities[j].entity;
      if (a == b)
        continue;

      if (is_collision_aabb(a->position, a->size, b->position, b->size))
      {
        physics->entities[i].on_collision(physics->entities[i].entity, physics->entities[j].entity);
        break;
      }
    }
}