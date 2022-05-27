#ifndef WIFI_CONNECT_H_
#define WIFI_CONNECT_H_

#include "sd_card_lib.h"
#include "esp_system.h"
#include "esp_event.h"
typedef void (*wifi_handler_t)(esp_event_base_t event_base, int32_t event_id, void* event_data);
#define SSID "Android"
#define PASS "123456788"

// extern int32_t temperature, humidity;
extern const char* file_disconnect_wifi_data;

void wifi_connection(void);
void wifi_handler_set_callback(void *cb);

#endif
