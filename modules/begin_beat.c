#include "modules/begin_beat.h"

void begin_beat__start(const BeginBeat_Config *config, BeginBeat_State *state) {
  if (0 < state->step)
    return;
  state->step = 1;
  if (config->started)
    config->started();
}

void begin_beat__poll(const BeginBeat_Config *config, BeginBeat_State *state) {
  uint8_t step = state->step;
  if (step <= 0)
    return;
  if (step <= 4)
    config->output(!!(step % 2));
  if (4 <= step) {
    state->step = 0;
    if (config->finished)
      config->finished();
  } else {
    state->step = step + 1;
  }
}
