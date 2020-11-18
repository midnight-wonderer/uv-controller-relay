#include "modules/tick_timer.h"

void tick_timer__start(const TickTimer_Config *config, TickTimer_State *state,
                       uint16_t count_value) {
  if (state->running)
    return;
  if (!count_value)
    return;
  state->count = count_value;
}

void tick_timer__poll(const TickTimer_Config *config, TickTimer_State *state) {
  uint16_t count = state->count;
  if (!count)
    return;
  if (!(state->running)) {
    state->running = true;
    if (config->started)
      config->started();
    return;
  }
  state->count = --count;
  if (0 < count)
    return;
  state->running = false;
  if (config->finished)
    config->finished();
}
