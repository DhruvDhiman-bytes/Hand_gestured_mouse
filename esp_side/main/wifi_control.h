#ifndef WIFI_CONTROL_H_
#define WIFI_CONTROL_H_

static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
esp_err_t wifi_init_sta(void);
#endif
