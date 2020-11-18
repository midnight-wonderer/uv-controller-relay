#include "modules/end_beat.h"

void end_beat__start(const EndBeat_Config *config, EndBeat_State *state) {
  if (0 < state->step)
    return;
  state->step = 1;
  if (config->started)
    config->started();
}

void end_beat__poll(const EndBeat_Config *config, EndBeat_State *state) {
  uint8_t step = state->step;
  if (step <= 0)
    return;
  if (step <= 8)
    config->output(!!(step % 2));
  if (8 <= step) {
    state->step = 0;
    if (config->finished)
      config->finished();
  } else {
    state->step = step + 1;
  }
}
