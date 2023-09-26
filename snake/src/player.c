#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "player.h"
#include "shader.h"
#include "glad/gl.h"
#include "renderer.h"
#include "collision.h"

#define TAIL_MAX_LEN 2048

typedef struct Player_t
{
  vec2 position;
  vec2 tail[TAIL_MAX_LEN];
  int tail_index;
  int tail_length;
  vec2 velocity;
  Renderer_t *renderer;
  bool dead;
} Player_t;

Player_t * Player_Init(void)
{
  Player_t * player = malloc(sizeof(Player_t));
  player->position[0] = 400.0f;
  player->position[1] = 300.0f;
  memset(player->tail, 0, sizeof(player->tail));
  player->tail_index = 0;
  player->tail_length = 10;

  for (int i = 0; i < player->tail_length; ++i)
  {
    player->tail[i][0] = -1.0f;
    player->tail[i][1] = -1.0f;
  }

  player->velocity[0] = 0.0f;
  player->velocity[1] = 0.0f;

  const char *vs =
      "#version 330 core\n"
      "layout (location = 0) in vec3 aPos;\n"
      "uniform mat4 model;\n"
      "uniform mat4 projection;\n"
      "void main()\n"
      "{\n"
      "   gl_Position = projection * model * vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n"
      "}\n";

  const char *fs =
      "#version 330 core\n"
      "out vec4 FragColor;\n"
      "void main()\n"
      "{\n"
      "FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
      "}\n";

  Shader_t *shader = Shader_Init();
  Shader_Load(shader, vs, fs);

  float vertices[] = {
      0.0f, 1.0f,
      1.0f, 0.0f,
      0.0f, 0.0f,

      0.0f, 1.0f,
      1.0f, 1.0f,
      1.0f, 0.0f};

  player->renderer = Renderer_Init(vertices, sizeof(vertices), shader);
  player->dead = false;

  return player;
}

void Player_Update(Player_t * player, float dt)
{
  vec2 new_pos;
  new_pos[0] = player->position[0] + player->velocity[0] * dt;
  new_pos[1] = player->position[1] + player->velocity[1] * dt;

  for (int i = 0; i < player->tail_length; ++i)
  {
    if (i == player->tail_index)
      continue;
    if (Collision_PointToPoint(player->tail[i], new_pos, 0.1f))
    {
      player->dead = true;
      player->velocity[0] = 0.0f;
      player->velocity[1] = 0.0f;
    }
  }
  float xdiff = player->position[0] - new_pos[0];
  float ydiff = player->position[1] - new_pos[1];

  if (xdiff > 0.1f || xdiff < -0.1f || ydiff > 0.1f || ydiff < -0.1f)
  {
    if (player->tail_index == player->tail_length)
    {
      player->tail_index = 0;
    }

    player->tail[player->tail_index][0] = player->position[0];
    player->tail[player->tail_index][1] = player->position[1];
    player->tail_index++;
  }

  player->position[0] = new_pos[0];
  player->position[1] = new_pos[1];

  vec2 player_size = { 10.0f, 10.0f };

  Renderer_RenderObject(player->renderer, player->position, player_size);

  for (int i = 0; i < player->tail_length; ++i)
  {
    if (player->tail[i][0] != -1.0f && player->tail[i][1] != -1.0f)
    {
      Renderer_RenderObject(player->renderer, player->tail[i], player_size);
    }
  }
}

void Player_SetVelocity(Player_t * player, vec2 velocity)
{
  player->velocity[0] = velocity[0];
  player->velocity[1] = velocity[1];
}

vec2 *Player_GetPosition(Player_t* player)
{
  return &player->position;
}

void Player_Eat(Player_t *player)
{
  for (int i = player->tail_length; i < player->tail_length + 10; ++i)
  {
    player->tail[i][0] = -1.0f;
    player->tail[i][1] = -1.0f;
  }

  player->tail_length += 10;
}

bool Player_IsDead(Player_t *player)
{
  return player->dead;
}

void Player_Delete(Player_t *player)
{
  Renderer_Delete(player->renderer);
  free(player);
}