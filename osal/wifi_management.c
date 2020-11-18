#include "osal/wifi_management.h"

void wifi_management__start(const WifiManagement_Config *config,
                            WifiManagement_State *state) {
  if (state->_status != _WIFI_MANAGEMENT__STARTING)
    return;
  state->_status = _WIFI_MANAGEMENT__CONNECTING;
  config->connect();
}

void wifi_management__handle_connection(const WifiManagement_Config *config,
                                        WifiManagement_State *state) {
  if (state->_status == _WIFI_MANAGEMENT__CONNECTED)
    return;
  state->_status = _WIFI_MANAGEMENT__CONNECTED;
  if (config->connected)
    config->connected();
}

void wifi_management__handle_disconnection(const WifiManagement_Config *config,
                                           WifiManagement_State *state) {
  if (state->_status == _WIFI_MANAGEMENT__CONNECTED)
    if (config->disconnected)
      config->disconnected();
  state->_status = _WIFI_MANAGEMENT__CONNECTING;
  config->connect();
}
