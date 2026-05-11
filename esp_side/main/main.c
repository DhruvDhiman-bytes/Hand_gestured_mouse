/*
 * In order to understand the struct or the member that are used
 * Plz refer the docs about the esp-idf API reference section for the information
 * about the struct and also the member of it.
 * In case you are not the author of the program plz make a pull request with all the
 * information about the pull changes and the reason behind it.
 *
 */



#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"
#include "hal/gpio_types.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"
#include "hal/spi_types.h"
#include "soc/clk_tree_defs.h"
#include <netinet/in.h>
#include "lwip/netdb.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"

// ======================== Space for defining the pin using macro ===========================

// pin defination for motor driver

#define M1_PIN_IN1 18
#define M1_PIN_IN2 19
#define M2_PIN_IN1 21
#define M2_PIN_IN2 22

// pin defination for SPI communication

#define MOSI_PIN 32
#define MISO_PIN 33
#define SPICLK_PIN 14
#define SPICS_PIN 5

#define PORT 3333

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
        .isr_cpu_id = 0
    };
    spi_bus_initialize(SPI2_HOST, &spi_bus_config, SPI_DMA_CH_AUTO);
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

void spi_slave_interface_init(void) {
    spi_slave_interface_config_t spi_salve_interface_config = {
        .spics_io_num = SPICS_PIN,
        .mode = 0,
    };
}

// ================= MOTOR CONTROL FUNCTIONS =======================

void motor_stop(void)
{
    gpio_set_level(M1_PIN_IN1, 0);
    gpio_set_level(M1_PIN_IN2, 0);

    gpio_set_level(M2_PIN_IN1, 0);
    gpio_set_level(M2_PIN_IN2, 0);
}

void motor_forward(void)
{
    gpio_set_level(M1_PIN_IN1, 1);
    gpio_set_level(M1_PIN_IN2, 0);

    gpio_set_level(M2_PIN_IN1, 1);
    gpio_set_level(M2_PIN_IN2, 0);
}

void motor_backward(void) {
    gpio_set_level(M1_PIN_IN1,0);
    gpio_set_level(M1_PIN_IN1, 1);

    gpio_set_level(M2_PIN_IN1, 0);
    gpio_set_level(M2_PIN_IN2, 1);
}

void motor_left(void) {
    gpio_set_level(M1_PIN_IN1, 0);
    gpio_set_level(M1_PIN_IN2, 1);

    gpio_set_level(M2_PIN_IN2, 1);
    gpio_set_level(M2_PIN_IN2, 0);
}

void motor_right(void) {
    gpio_set_level(M1_PIN_IN1, 1);
    gpio_set_level(M1_PIN_IN2, 0);

    gpio_set_level(M2_PIN_IN1, 0);
    gpio_set_level(M2_PIN_IN2, 1);
}

// ================= PROCESS RECEIVED COMMAND ===============

void process_command(char *command) {
    motor_stop();

    if(strcmp(command, "FORWARD") == 0) {
        motor_forward();
    }

    else if(strcmp(command, "BACKWARD") == 0) {
        motor_backward();
    }

    else if(strcmp(command, "LEFT") == 0) {
        motor_left();
    }

    else if(strcmp(command, "RIGHT") == 0) {
        motor_right();
    }

    else if(strcmp(command, "STOP") == 0) {
        motor_stop();
    }
}


// ================== TCP SERVER ====================

void start_tcp_server(void) {

    char rx_buffer[128];

    struct sockaddr_in server_addr;

    int listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listen_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    while(1) {
        struct sockaddr_in client_addr;

        unsigned int client_addr_len = sizeof(client_addr);

        int sock = accept(listen_sock, (struct sockaddr *)&client_addr, &client_addr_len);

        while(1) {
            int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            if (len < 0) {
                break;
            }
            else if(len == 0) {
                break;
            }
            else {
                rx_buffer[len] = 0;

                char *newline = strchr(rx_buffer, '\n');

                if(newline) {
                    *newline = 0;
                }
                process_command(rx_buffer);
            }
        }
        close(sock);
    }
}

void app_main(void)
{
    // calling on the function for initialzing the config for the pins
    m1_pin_init();
    m2_pin_init();

    start_tcp_server();
}
