#ifndef __APP__TICK_TIMER
#define __APP__TICK_TIMER

#include <stdbool.h>
#include <stdint.h>

typedef struct _tick_timer__config {
  void (*started)(void);
  void (*finished)(void);
} TickTimer_Config;

typedef struct _tick_timer__state {
  uint16_t count;
  bool running;
} TickTimer_State;

#define tick_timer__init(state)                                                \
  do {                                                                         \
    (state)->count = 0;                                                        \
    (state)->running = false;                                                  \
  } while (0)

void tick_timer__start(const TickTimer_Config *config, TickTimer_State *state,
                       uint16_t count_value);
void tick_timer__poll(const TickTimer_Config *config, TickTimer_State *state);

#endif
