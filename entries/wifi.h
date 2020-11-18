#ifndef __ENTRY__WIFI
#define __ENTRY__WIFI

#ifdef __cplusplus
extern "C" {
#endif

// provides
void wifi_setup(void);

// needs
void wifi_connected(void);
void wifi_disconnected(void);

#ifdef __cplusplus
}
#endif

#endif
