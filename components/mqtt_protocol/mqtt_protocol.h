#ifndef MQTT_PROTOCOL_H_
#define MQTT_PROTOCOL_H_

void mqtt_app_start(void);
void wifi_connect_read_sd_card_task(void *pvParameters);
void mqtt_app_reconnect(void);
void mqtt_app_stop(void);
int mqtt_app_pulish(const char *topic, const char *data, int len, int qos, int retain);
void mqtt_app_disconnect(void);
#endif
