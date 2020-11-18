#include "./uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stddef.h>

// https://github.com/espressif/ESP8266_RTOS_SDK/pull/1006
#include "driver/uart.h"

#define BUF_SIZE (128)

// Configure a temporary buffer for the incoming data
static uint8_t data[BUF_SIZE];

static void echo_task() {
  // Configure parameters of an UART driver,
  // communication pins and install the driver
  uart_config_t uart_config = {
      .baud_rate = 115200,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
  };
  uart_param_config(UART_NUM_0, &uart_config);
  uart_driver_install(UART_NUM_0, BUF_SIZE * 2, 0, 0, NULL, 0);

  // Configure a temporary buffer for the incoming data
  // uint8_t *data = (uint8_t *)malloc(BUF_SIZE);

  while (1) {
    // Read data from the UART
    int len =
        uart_read_bytes(UART_NUM_0, data, BUF_SIZE, 20 / portTICK_RATE_MS);
    // Write data back to the UART
    uart_write_bytes(UART_NUM_0, (const char *)data, len);
    uart_write_bytes(UART_NUM_0, (const char *)data, len);
  }
}

void uart_setup(void) {
  xTaskCreate(echo_task, "uart_echo_task", 1024, NULL, 10, NULL);
}
