 #include <stdio.h>
#include "driver/gpio.h"
#include "hal/gpio_types.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"

// ======================== Space for defining the pin using macro ===========================

#define M1_PIN_IN1 18
#define M1_PIN_IN2 19
#define M2_PIN_IN1 21
#define M2_PIN_IN2 22

// ======================= Space for writing the configuration for the pins =======================

void m1_pin_init(void) {
    gpio_config_t m1_config = {
        .pin_bit_mask = (1ULL << M1_PIN_IN1) | (1ULL << M1_PIN_IN2),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config(&m1_config);
}

void m2_pin_init(void) {
    gpio_config_t m2_config = {
        .pin_bit_mask = (1ULL << M2_PIN_IN1) | (1ULL << M2_PIN_IN2),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config(&m2_config);
}

// ======================== Space for any function defined by the user =============================




void app_main(void)
{
    // calling on the function for initialzing the config for the pins
    m1_pin_init();
    m2_pin_init();

}
