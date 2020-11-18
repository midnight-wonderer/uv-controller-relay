#include "osal/udp_management.h"

void udp_management__listen(const UdpManagement_Config *config,
                            UdpManagement_State *state) {
  if (state->_status != _UDP_MANAGEMENT__UNINITIALIZED)
    return;
  state->_status = _UDP_MANAGEMENT__LISTENING;
  config->listen();
}

void udp_management__close(const UdpManagement_Config *config,
                           UdpManagement_State *state) {
  if (state->_status == _UDP_MANAGEMENT__UNINITIALIZED)
    return;
  state->_status = _UDP_MANAGEMENT__UNINITIALIZED;
  config->close();
}

void udp_management__poll(const UdpManagement_Config *config,
                          UdpManagement_State *state) {
  if (state->_status != _UDP_MANAGEMENT__LISTENING)
    return;
  int8_t length = config->poll(state->rx_buffer);
  if (length <= 0)
    return;
  state->_status = _UDP_MANAGEMENT__HALT;
  config->received(state->rx_buffer, length);
}

void udp_management__reply(const UdpManagement_Config *config,
                           UdpManagement_State *state, uint8_t *tx_buffer,
                           uint8_t length) {
  if (state->_status != _UDP_MANAGEMENT__HALT)
    return;
  config->reply(tx_buffer, length);
  state->_status = _UDP_MANAGEMENT__LISTENING;
}
