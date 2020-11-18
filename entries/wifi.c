#include "entries/wifi.h"
#include "config/secrets.h"
#include "osal/wifi_management.h"

#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"
#include <stdbool.h>

#include "esp_log.h"

// ###############################
// #        OSAL BINDINGS        #
// ###############################

static void osal_connect_handler(void);
static const WifiManagement_Config osal_management_config = {
    .connect = &osal_connect_handler,
    .connected = &wifi_connected,
    .disconnected = &wifi_disconnected,
};
static WifiManagement_State osal_management_state;
static void osal_connect_handler(void) { esp_wifi_connect(); }

// ####################################
// #        ESP PLATFORM LAYER        #
// ####################################

static EventGroupHandle_t wifi_event_group;

static esp_err_t event_handler(void *context, system_event_t *event) {
  switch (event->event_id) {
  case SYSTEM_EVENT_STA_START:
    // not supported: https://github.com/espressif/esp-idf/issues/702
    // esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCOL_11G |
    // WIFI_PROTOCOL_11N);
    esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCOL_11B |
                                               WIFI_PROTOCOL_11G |
                                               WIFI_PROTOCOL_11N);
    tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, "UV Controller");
    wifi_management__start(&osal_management_config, &osal_management_state);
    break;
  case SYSTEM_EVENT_STA_GOT_IP:
    // xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    ESP_LOGI("", "Got IP");
    wifi_management__handle_connection(&osal_management_config,
                                       &osal_management_state);
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    // xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
    ESP_LOGI("", "Disconn");
    wifi_management__handle_disconnection(&osal_management_config,
                                          &osal_management_state);
    break;
  default:
    break;
  }
  return ESP_OK;
}

// ###############################
// #        MAIN BINDINGS        #
// ###############################

void wifi_setup(void) {
  wifi_management__init(&osal_management_state);
  wifi_event_group = xEventGroupCreate();
  esp_netif_init();
  esp_event_loop_init(event_handler, NULL);
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  cfg.nvs_enable = false;
  esp_wifi_init(&cfg);
  esp_wifi_set_ps(WIFI_PS_NONE);
  wifi_config_t wifi_config = {
      .sta =
          {
              .ssid = CONFIG__WIFI_SSID,
              .password = CONFIG__WIFI_PASSWORD,
              .scan_method = WIFI_ALL_CHANNEL_SCAN,
              .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
              .threshold =
                  {
                      .authmode = WIFI_AUTH_WPA2_PSK,
                  },
          },
  };
  esp_wifi_set_mode(WIFI_MODE_STA);
  esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
  esp_wifi_start();
}
