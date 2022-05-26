#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "mqtt_client.h"
#include "nvs_flash.h"
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include "lwip/err.h"
#include "lwip/sys.h"

#include "mqtt_protocol.h"
#include "sd_card_lib.h"
#include "wifi_connect.h"


static wifi_handler_t wifi_handler = NULL;
extern EventGroupHandle_t sd_card_write_event_group;
// void wifi_disconect_write_sd_card_task(void* pvParameters)
// {
//     printf("DO TASK WRITE\n");
//     /* if(xreadHandle != NULL) {
//         vTaskDelete(xreadHandle);
//         printf("DELETED TASK READ\n");
//     } */
//     for (;;) {
//         EventBits_t bits = xEventGroupWaitBits(sd_card_write_event_group,
//                                                 SD_CARD_WRITE_BIT,
//                                                 pdFALSE,
//                                                 pdFALSE,
//                                                 portMAX_DELAY);
//         sd_card_write(file_disconnect_wifi_data, temperature, humidity);
//         printf("WRITEN TO SD CARD\n");
//         esp_wifi_connect();
//         ESP_LOGI(TAG, "retry to connect to the AP");
//         xwriteHandle = NULL;
//         printf("REMOVED WRITE TASK\n");
//         vTaskDelete(NULL);
//     }
// }

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{

    // if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
    //     if (s_retry_num < 5) {
    //         esp_wifi_connect();
    //         s_retry_num++;
    //         ESP_LOGI(TAG, "retry to connect to the AP");
    //     } else {
    //         if (xwriteHandle == NULL) {
    //             xTaskCreate(wifi_disconect_write_sd_card_task, "wifi_disconect_write_sd_card_task", 4096, NULL, 1, &xwriteHandle);
    //             printf("CREATED TASK WRITE\n");
    //         }
    //         xEventGroupSetBits(sd_card_write_event_group, SD_CARD_WRITE_BIT);
    //         printf("SETED WRITE BIT\n");
    //     }
    //     ESP_LOGI(TAG, "connect to the AP fail");
    // } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    //     ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
    //     ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
    //     s_retry_num = 0;
    //     struct stat st;
    //     if (stat(file_disconnect_wifi_data, &st) == 0) {
    //         printf("FILE EXISTS\n");
    //         esp_mqtt_client_reconnect(even_data_parameters);
    //         if (xreadHandle == NULL) {
    //             xTaskCreate(wifi_connect_read_sd_card_task, "wifi_connect_read_sd_card_task", 4096, NULL, 1, &xreadHandle);
    //             printf("CREATED TASK READ\n");
    //         }
    //         xEventGroupSetBits(sd_card_write_event_group, SD_CARD_READ_BIT);
    //         printf("SETED BIT READ\n");
    //     } else {
    //         printf("FILE NOT EXISTS\n");
    //     }
    // }
    wifi_handler(event_handler_arg, event_base, event_id, event_data);
}

void wifi_connection()
{
    sd_card_write_event_group = xEventGroupCreate();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation);
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = SSID,
            .password = PASS}};
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    esp_wifi_start();
    esp_wifi_connect();
}
void wifi_handler_set_callback(void *cb)
{
    wifi_handler = cb;
}
