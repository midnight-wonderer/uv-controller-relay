#ifndef __OSAL__WIFI_MANAGEMENT
#define __OSAL__WIFI_MANAGEMENT

#include <stdint.h>

typedef struct __wifi_management__config {
  void (*connect)(void);
  void (*connected)(void);
  void (*disconnected)(void);
} WifiManagement_Config;

typedef enum __wifi_management__internal_status {
  _WIFI_MANAGEMENT__STARTING,
  _WIFI_MANAGEMENT__CONNECTING,
  _WIFI_MANAGEMENT__CONNECTED,
} _WifiManagement_InternalStatus;

typedef struct __wifi_management__state {
  _WifiManagement_InternalStatus _status;
} WifiManagement_State;

#define wifi_management__init(state)                                           \
  do {                                                                         \
    (state)->_status = _WIFI_MANAGEMENT__STARTING;                             \
  } while (0)

void wifi_management__start(const WifiManagement_Config *config,
                            WifiManagement_State *state);

void wifi_management__handle_connection(const WifiManagement_Config *config,
                                        WifiManagement_State *state);

void wifi_management__handle_disconnection(const WifiManagement_Config *config,
                                           WifiManagement_State *state);

#endif
