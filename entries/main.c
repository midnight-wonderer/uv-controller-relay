#include "config/wemos_pin.h"
#include "driver/gpio.h"

#define BUZZER_PIN D5
#define RELAY_PIN D2

// ###############################
// #     MEMORY ALLOCATIONS      #
// ###############################
#include "modules/begin_beat.h"
#include "modules/end_beat.h"
#include "modules/tick_timer.h"

static void main_timer_started(void);
static void main_timer_finished(void);
static const TickTimer_Config main_timer_config = {
    .started = &main_timer_started,
    .finished = &main_timer_finished,
};
static TickTimer_State main_timer_state;

static void beat_output(bool turn_on);
static void begin_beat_finished(void);
static void begin_beat_started(void);
static void end_beat_finished(void);
static const BeginBeat_Config begin_beat_config = {
    .output = &beat_output,
    .finished = &begin_beat_finished,
    .started = &begin_beat_started,
};
static const EndBeat_Config end_beat_config = {
    .output = &beat_output,
    .finished = &end_beat_finished,
};
static BeginBeat_State begin_beat_state;
static EndBeat_State end_beat_state;

// ###############################
// #           SHARED            #
// ###############################

// shared timers
// - 60 ms
// - 500 ms
// - 1 s
// derived params
#define TIMER_GCD (20)   // ms
#define TIMER_LCM (3000) // ms
#define TIMER_60_SCALER (60 / TIMER_GCD)
#define TIMER_500_SCALER (500 / TIMER_GCD)
#define TIMER_SCALER_MODULAR (TIMER_LCM / TIMER_GCD)
#define TIMER_TICK_PER_SECOND (1000 / 500)
#define TIMER_SECOND_TO_TICK(second) ((second)*TIMER_TICK_PER_SECOND)

// ###############################
// #         EVENT FLOWS         #
// ###############################
#include <stdbool.h>

static bool waiting_for_command = true;
static void beat_output(bool turn_on) { gpio_set_level(BUZZER_PIN, !turn_on); }
static void begin_beat_started(void) { waiting_for_command = false; }
static void begin_beat_finished(void) {
  tick_timer__start(&main_timer_config, &main_timer_state,
                    TIMER_SECOND_TO_TICK(3));
}
static void main_timer_started(void) { gpio_set_level(RELAY_PIN, true); }
static void main_timer_finished(void) {
  gpio_set_level(RELAY_PIN, false);
  end_beat__start(&end_beat_config, &end_beat_state);
}
static void end_beat_finished(void) { waiting_for_command = true; }

#include "entries/udp_server.h"
#include "entries/wifi.h"

void udp_received(uint8_t *buffer, uint8_t length) {
  if (waiting_for_command)
    begin_beat__start(&begin_beat_config, &begin_beat_state);
  // gpio_set_level(BUZZER_PIN, !gpio_get_level(BUZZER_PIN));
  udp_reply(buffer, 0);
}

void wifi_connected(void) { udp_listen(); }
void wifi_disconnected(void) { udp_close(); }

// ###############################
// #       PLATFORM SETUP        #
// ###############################
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

static xTimerHandle shared_signal;
static void tick(xTimerHandle);

void app_main() {
  gpio_config_t io_conf;
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  gpio_set_level(BUZZER_PIN, true);
  gpio_set_level(RELAY_PIN, false);
  io_conf.pin_bit_mask = (1ULL << RELAY_PIN);
  io_conf.mode = GPIO_MODE_OUTPUT;
  gpio_config(&io_conf);
  io_conf.pin_bit_mask = (1ULL << BUZZER_PIN);
  io_conf.mode = GPIO_MODE_OUTPUT_OD;
  gpio_config(&io_conf);

  begin_beat__init(&begin_beat_state);
  end_beat__init(&end_beat_state);
  tick_timer__init(&main_timer_state);

  // Shared low frequency signal
  shared_signal =
      xTimerCreate("A", pdMS_TO_TICKS(TIMER_GCD), pdTRUE, (void *)0, tick);
  xTimerStart(shared_signal, 0);

  wifi_setup();
  udp_setup();
}

void vApplicationIdleHook(void) { udp_poll(); }

static uint8_t tick_prescaler = 0;
static void tick(xTimerHandle _timer) {
  uint8_t step = tick_prescaler;
  tick_prescaler = (tick_prescaler + 1) % TIMER_SCALER_MODULAR;
  if (step % TIMER_60_SCALER == 0) {
    begin_beat__poll(&begin_beat_config, &begin_beat_state);
    end_beat__poll(&end_beat_config, &end_beat_state);
  }
  if (step % TIMER_500_SCALER == 0)
    tick_timer__poll(&main_timer_config, &main_timer_state);
}
