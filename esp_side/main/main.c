/*
 *
 * In order to understand the struct or the member that are used
 * Plz refer the docs about the esp-idf API reference section for the information
 * about the struct and also the member of it.
 * In case you are not the author of the program plz make a pull request with all the
 * information about the pull changes and the reason behind it.
 *
 */



#include <stdio.h>
#include "driver/gpio.h"
#include "hal/gpio_types.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"
#include "soc/clk_tree_defs.h"

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
#define SPICS_PIN 15

// ======================= Space for writing the configuration for the pins =======================

// init function for motor drivers

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
        .data7_io_num = -1,
        .isr_cpu_id = 0
    };
}

// init function for spi communication master side

void spi_device_interface_init(void) {
    spi_device_interface_config_t spi_device_interface_config = {
        .command_bits = 8,
        .address_bits  = 0,
        .dummy_bits = 0,
        .mode = 0,
        .clock_source = SPI_CLK_SRC_DEFAULT,
        .clock_speed_hz = 40000,
        .input_delay_ns = 0,
    };
}

void spi_transaction_init(void) {
    spi_transaction_t spi_transaction_config = {
        .cmd = 8,
        .addr = 0,
        .length = 32,
        .rxlength = 64,
        .override_freq_hz = 0,
        .tx_buffer = NULL,
        .rx_buffer = NULL,
    };
}

// init function for spi communication slave side

//TODO -> Use a different hand shake before the device starts to receive data from the master

void spi_slave_transaction_init (void) {
    spi_slave_transaction_t spi_salve_transaction_config = {
        .length = 32,
        .trans_len = 32,
        .tx_buffer = 0,
        .rx_buffer = 0
    };
}

void spi_slace_interface_init(void) {
    spi_slave_interface_config_t spi_salve_interface_config = {
        .spics_io_num = ,
    };
}
// ======================== Space for any function defined by the user =============================




void app_main(void)
{
    // calling on the function for initialzing the config for the pins
    m1_pin_init();
    m2_pin_init();

}
