#ifndef __OSAL__UDP_MANAGEMENT
#define __OSAL__UDP_MANAGEMENT

#include <stdint.h>

#define UDP_MANAGEMENT__BUFFER_SIZE 96

typedef struct __udp_management__config {
  void (*listen)(void);
  void (*close)(void);
  int8_t (*poll)(uint8_t *rx_buffer);
  void (*reply)(uint8_t *tx_buffer, uint8_t length);
  void (*received)(uint8_t *data, uint8_t length);
} UdpManagement_Config;

typedef enum __udp_management__internal_status {
  _UDP_MANAGEMENT__UNINITIALIZED,
  _UDP_MANAGEMENT__LISTENING,
  _UDP_MANAGEMENT__HALT,
} _UdpManagement_InternalStatus;

typedef struct __udp_management__state {
  _UdpManagement_InternalStatus _status;
  uint8_t *rx_buffer;
} UdpManagement_State;

#define udp_management__init(state, externally_allocated_rx_buffer)            \
  do {                                                                         \
    (state)->_status = _UDP_MANAGEMENT__UNINITIALIZED;                         \
    (state)->rx_buffer = (externally_allocated_rx_buffer);                     \
  } while (0)

void udp_management__listen(const UdpManagement_Config *config,
                            UdpManagement_State *state);

void udp_management__close(const UdpManagement_Config *config,
                           UdpManagement_State *state);

void udp_management__poll(const UdpManagement_Config *config,
                          UdpManagement_State *state);

void udp_management__reply(const UdpManagement_Config *config,
                           UdpManagement_State *state, uint8_t *tx_buffer,
                           uint8_t length);

#endif
