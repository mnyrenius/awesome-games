#pragma once

#include <stdbool.h>
#pragma once

#include "types.h"

typedef struct Global_Time_t
{
  f32 dt;
  f32 now;
} Global_Time_t;

extern Global_Time_t Global_Time;
void Global_Time_Update(void);
