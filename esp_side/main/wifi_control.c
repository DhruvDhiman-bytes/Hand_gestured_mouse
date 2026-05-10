/*
 * This file intent is to write the code for esp32 wifi connection
 * For any information about the struct or the member of the struct
 * plz refer the docs of esp-idf programming guide.
 * ===================== NOTE TO NON-AUTHOR =========================
 * If you like make a pull request pull do it but explaining your code
 * and the choices you make for it, explain the meaning behind them too
 */

#include <stdio.h>
#include "esp_wifi_types_generic.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"


// ================ Space for global variables and macro ==========================

#define WIFI_SSID "Dhruv Dhiman"
#define WIFI_PASS "dadada123456789"
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;
static const char *TAG = "wifi";

// ================ space for init function or user made function ==================

static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {

    if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    }
    else if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if(s_retry_num < 30) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGW(TAG,"Trying to connect to wifi");
        }
        else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGE(TAG, "Failed to connect to wifi");
    }
    else if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}
