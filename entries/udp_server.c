#include "entries/udp_server.h"
#include "config/secrets.h"
#include "osal/udp_management.h"

#include "lwip/sockets.h"

#define UDP_LISTEN_PORT (CONFIG__UDP_PORT)

// ###############################
// #        OSAL BINDINGS        #
// ###############################

static void osal_listen(void);
static void osal_close(void);
static int8_t osal_poll(uint8_t *);
static void osal_reply(uint8_t *, uint8_t);
static const UdpManagement_Config osal_management_config = {
    .listen = &osal_listen,
    .close = &osal_close,
    .poll = &osal_poll,
    .reply = &osal_reply,
    .received = &udp_received,
};
static UdpManagement_State osal_management_state;
static uint8_t packet_receive_buffer[UDP_MANAGEMENT__BUFFER_SIZE];

// ####################################
// #        ESP PLATFORM LAYER        #
// ####################################

static struct sockaddr_in listen_to = {
    .sin_family = AF_INET,
};

static int socket_handler;

static void osal_listen(void) {
  socket_handler = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  bind(socket_handler, (struct sockaddr *)&listen_to, sizeof(listen_to));
}

static struct sockaddr_in last_address;
static socklen_t address_length = sizeof(last_address);

static int8_t osal_poll(uint8_t *buffer) {
  int length =
      recvfrom(socket_handler, buffer, UDP_MANAGEMENT__BUFFER_SIZE - 1,
               MSG_DONTWAIT, (struct sockaddr *)&last_address, &address_length);
  return (int8_t)length;
}

static void osal_close(void) {
  shutdown(socket_handler, 0);
  close(socket_handler);
}

static void osal_reply(uint8_t *tx_buffer, uint8_t length) {
  sendto(socket_handler, tx_buffer, length, MSG_DONTWAIT,
         (struct sockaddr *)&last_address, address_length);
}

// ###############################
// #        MAIN BINDINGS        #
// ###############################

void udp_setup(void) {
  listen_to.sin_addr.s_addr = htonl(INADDR_ANY);
  listen_to.sin_port = htons(UDP_LISTEN_PORT);
  udp_management__init(&osal_management_state, packet_receive_buffer);
}

void udp_poll(void) {
  udp_management__poll(&osal_management_config, &osal_management_state);
}

void udp_listen(void) {
  udp_management__listen(&osal_management_config, &osal_management_state);
}

void udp_close(void) {
  udp_management__close(&osal_management_config, &osal_management_state);
}

void udp_reply(uint8_t *buffer, uint8_t length) {
  udp_management__reply(&osal_management_config, &osal_management_state, buffer,
                        length);
}
