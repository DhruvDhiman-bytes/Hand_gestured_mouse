 #include <stdio.h>
#include "driver/gpio.h"
#include "hal/gpio_types.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"

// ======================== Space for defining the pin using macro ===========================

// pin defination for motor driver

#define M1_PIN_IN1 18
#define M1_PIN_IN2 19
#define M2_PIN_IN1 21
#define M2_PIN_IN2 22

// pin defination for SPI communication

#define MOSI_PIN 32
#define MISO_PIN 33
#define SPICLK_PIN 18

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

void spi_bus_init(void) {
    spi_bus_config_t spi_bus_config = {
        .mosi_io_num = MOSI_PIN,
        .miso_io_num = MISO_PIN,
        .sclk_io_num = SPICLK_PIN,
        .data0_io_num = -1,
        .data1_io_num = -1,
        .data2_io_num = -1,
        .data3_io_num = -1,
        .data4_io_num = -1,
        .data5_io_num = -1,
        .data6_io_num = -1,
        .data7_io_num = -1
    };
}

void spi_device_interface__init(void) {
    spi_device_interface_config_t spi_device_interface_config = {

    };
}

// ======================== Space for any function defined by the user =============================




void app_main(void)
{
    // calling on the function for initialzing the config for the pins
    m1_pin_init();
    m2_pin_init();

}
