#include "esp_event.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"

#include "cJSON.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "mqtt_protocol.h"
#include "sd_card_lib.h"
#include "wifi_connect.h"

EventGroupHandle_t sd_card_write_event_group;
uint32_t id = 0;
static const char *TAG = "wifi connection";
static int s_retry_num = 0;
int32_t temperature = 30, humidity = 90;
TaskHandle_t xwriteHandle = NULL;
TaskHandle_t xreadHandle = NULL;

#define SD_CARD_WRITE_BIT BIT0
#define SD_CARD_READ_BIT BIT1
void wifi_disconect_write_sd_card_task(void *pvParameters)
{
    printf("DO TASK WRITE\n");
    /* if(xreadHandle != NULL) {
        vTaskDelete(xreadHandle);
        printf("DELETED TASK READ\n");
    } */
    for (;;)
    {
        xEventGroupWaitBits(sd_card_write_event_group, SD_CARD_WRITE_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
        sd_card_write(temperature, humidity);
        printf("WRITEN TO SD CARD\n");
        esp_wifi_connect();
        ESP_LOGI(TAG, "retry to connect to the AP");
        xwriteHandle = NULL;
        printf("REMOVED WRITE TASK\n");
        vTaskDelete(NULL);
    }
}
void wifi_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < 5)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            if (xwriteHandle == NULL)
            {
                xTaskCreate(wifi_disconect_write_sd_card_task, "wifi_disconect_write_sd_card_task", 4096, NULL, 1, &xwriteHandle);
                printf("CREATED TASK WRITE\n");
            }
            xEventGroupSetBits(sd_card_write_event_group, SD_CARD_WRITE_BIT);
            printf("SETED WRITE BIT\n");
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        struct stat st;
        if (stat(file_disconnect_wifi_data, &st) == 0)
        {
            printf("FILE EXISTS\n");
            // esp_mqtt_client_reconnect(even_data_parameters);
            mqtt_app_reconnect();
            if (xreadHandle == NULL)
            {
                xTaskCreate(wifi_connect_read_sd_card_task, "wifi_connect_read_sd_card_task", 4096, NULL, 1, &xreadHandle);
                printf("CREATED TASK READ\n");
            }
            xEventGroupSetBits(sd_card_write_event_group, SD_CARD_READ_BIT);
            printf("SETED BIT READ\n");
        }
        else
        {
            printf("FILE NOT EXISTS\n");
        }
    }
}
void wifi_connect_read_sd_card_task(void *pvParameters)
{
    printf("DO TASK READ\n");
    if (xwriteHandle != NULL)
    {
        vTaskDelete(xwriteHandle);
        printf("DELETED TASK WRITE\n");
    }
    static uint32_t line = 2;
    printf("ID: %d\n", id);
    for (;;)
    {
        xEventGroupWaitBits(sd_card_write_event_group, SD_CARD_READ_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
        for (int j = 0; j < id; j++)
        {
            messages_data_t messages_send = sd_card_read(line);
            printf("READED FROM SD CARD\n");
            char *json_string_send = json_creat(messages_send.id, messages_send.temperature, messages_send.humidity);
            printf("%s\n", json_string_send);
            printf("GIA TRI ID: %d\n", id);
            printf("GIA TRI LINE: %d\n", line);
            int check_publish = mqtt_app_pulish("my_topic", json_string_send, 0, 1, 0);
            printf("CHECK_PUBLISH = %d PUBLISHED TO MY_TOPIC\n", check_publish);
            printf("PUBLISHED TO MY_TOPIC\n");
            line++;
        }
        line = 2;
        id = 0;
        unlink(file_disconnect_wifi_data);
        printf("REMOVED FILE\n");
        xreadHandle = NULL;
        printf("REMOVED READ TASK\n");
        vTaskDelete(NULL);
    }
}
void app_main(void)
{
    nvs_flash_init();
    sd_card_init();
    wifi_handler_set_callback(wifi_handler);
    wifi_connection();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    printf("WIFI was initiated ...........\n");
    mqtt_app_start();
    printf("Init MQTT\n");
}