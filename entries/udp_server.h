#ifndef __ENTRY__UDP_SERVER
#define __ENTRY__UDP_SERVER

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// provides
void udp_setup(void);
void udp_poll(void);
void udp_listen(void);
void udp_close(void);
void udp_reply(uint8_t *buffer, uint8_t length);

// needs
void udp_received(uint8_t *buffer, uint8_t length);

#ifdef __cplusplus
}
#endif

#endif
