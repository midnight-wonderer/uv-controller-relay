#ifndef __APP__BEGIN_BEAT
#define __APP__BEGIN_BEAT

#include <stdbool.h>
#include <stdint.h>

typedef struct _begin_beat__config {
  void (*output)(bool turn_on);
  void (*started)(void);
  void (*finished)(void);
} BeginBeat_Config;

typedef struct _begin_beat__state {
  uint8_t step;
} BeginBeat_State;

#define begin_beat__init(state)                                                \
  do {                                                                         \
    (state)->step = 0;                                                         \
  } while (0)

void begin_beat__start(const BeginBeat_Config *config, BeginBeat_State *state);
void begin_beat__poll(const BeginBeat_Config *config, BeginBeat_State *state);

#endif
