#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "player.h"
#include "shader.h"
#include "glad/gl.h"
#include "renderer.h"
#include "collision.h"
#include "util.h"

#define TAIL_MAX_LEN 2048

typedef struct Player_t
{
  vec2 position;
  vec2 tail[TAIL_MAX_LEN];
  int tail_index;
  int tail_length;
  vec2 velocity;
  Renderer_t *renderer;
  Shader_t *shader;
  bool dead;
  bool invincible;
  float invincible_timer;
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

  float vertices[] = {
      0.0f, 1.0f,
      1.0f, 0.0f,
      0.0f, 0.0f,

      0.0f, 1.0f,
      1.0f, 1.0f,
      1.0f, 0.0f};

  const char vs[] =
  {
    #include "shaders/generic_object.vs.data"
    , 0
  };
  const char fs[] =
  {
    #include "shaders/player.fs.data"
    , 0
  };
  player->shader = Shader_Init();
  Shader_Load(player->shader, vs, fs);


  player->renderer = Renderer_Init_With_Shader(vertices, sizeof(vertices), player->shader);
  player->dead = false;
  player->invincible = false;
  player->invincible_timer = 0.0f;

  return player;
}

void Player_Update(Player_t * player, float dt)
{
  if (player->invincible)
  {
    player->invincible_timer -= dt;
    if (player->invincible_timer <= 0.0f)
    {
      player->invincible = false;
    }
  }

  vec2 new_pos;
  new_pos[0] = player->position[0] + player->velocity[0] * dt;
  new_pos[1] = player->position[1] + player->velocity[1] * dt;

  if (player->invincible)
  {
    if (new_pos[0] > 800.0f)
    {
      new_pos[0] = 0.0f;
    }
    else if (new_pos[0] < 0.0f)
    {
      new_pos[0] = 800.0f;
    }
    else if (new_pos[1] > 540.0f)
    {
      new_pos[1] = 0.0f;
    }
    else if (new_pos[1] < 0.0f)
    {
      new_pos[1] = 540.0f;
    }
  }

  if (!player->invincible)
  {
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
  vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

  Renderer_RenderObject(player->renderer, player->position, player_size, color);
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, (GLint*)&vp);
  vec2 head_pos = { vp[2]/800.0 * player->position[0], vp[3]/600.0 * player->position[1]};
  Shader_Use(player->shader);
  Shader_SetVec2(player->shader, "head_pos", head_pos);
  vec2 screen_res = { vp[2], vp[3]};
  Shader_SetVec2(player->shader, "screen_res", screen_res);
  for (int i = 0; i < player->tail_length; ++i)
  {
    if (player->tail[i][0] != -1.0f && player->tail[i][1] != -1.0f)
    {
      Renderer_RenderObject(player->renderer, player->tail[i], player_size, color);
    }
  }
}

void Player_Move(Player_t * player, Player_Movement_t movement)
{
  switch (movement)
  {
  case PLAYER_MOVEMENT_UP:
    player->velocity[0] = 0.0f;
    player->velocity[1] = -100.0f;
    break;
  case PLAYER_MOVEMENT_DOWN:
    player->velocity[0] = 0.0f;
    player->velocity[1] = 100.0f;
    break;
  case PLAYER_MOVEMENT_LEFT:
    player->velocity[0] = -100.0f;
    player->velocity[1] = 0.0f;
    break;
  case PLAYER_MOVEMENT_RIGHT:
    player->velocity[0] = 100.0f;
    player->velocity[1] = 0.0f;
    break;
  case PLAYER_MOVEMENT_STOP:
    player->velocity[0] = 0.0f;
    player->velocity[1] = 0.0f;
    break;

  default:
    break;
  }
}

vec2 *Player_GetPosition(Player_t* player)
{
  return &player->position;
}

void Player_Eat(Player_t *player, bool powerup)
{
  if (powerup)
  {
    player->invincible = true;
    player->invincible_timer = 10.0f;
  }

  for (int i = player->tail_length; i < player->tail_length + 10; ++i)
  {
    player->tail[i][0] = -1.0f;
    player->tail[i][1] = -1.0f;
  }

  player->tail_length += 25;
}

bool Player_IsDead(Player_t *player)
{
  return player->dead;
}

bool Player_IsInvincible(Player_t *player)
{
  return player->invincible;
}

void Player_Delete(Player_t *player)
{
  Renderer_Delete(player->renderer);
  free(player);
}