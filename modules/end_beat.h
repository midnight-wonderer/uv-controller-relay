#ifndef __APP__END_BEAT
#define __APP__END_BEAT

#include <stdbool.h>
#include <stdint.h>

typedef struct _end_beat__config {
  void (*output)(bool turn_on);
  void (*started)(void);
  void (*finished)(void);
} EndBeat_Config;

typedef struct _end_beat__state {
  uint8_t step;
} EndBeat_State;

#define end_beat__init(state)                                                \
  do {                                                                         \
    (state)->step = 0;                                                         \
  } while (0)

void end_beat__start(const EndBeat_Config *config, EndBeat_State *state);
void end_beat__poll(const EndBeat_Config *config, EndBeat_State *state);

#endif
