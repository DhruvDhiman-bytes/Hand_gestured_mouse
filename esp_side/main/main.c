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
#include "driver/uart.h"
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

// UART setting
#define UART_TX_PIN 17
#define UART_RX_PIN 16

#define UART_PORT UART_NUM_2
#define UART_BAUD_RATE 9600

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
    gpio_set_level(M1_PIN_IN2, 1);

    gpio_set_level(M2_PIN_IN1, 0);
    gpio_set_level(M2_PIN_IN2, 1);
}

void motor_left(void) {
    gpio_set_level(M1_PIN_IN1, 0);
    gpio_set_level(M1_PIN_IN2, 1);

    gpio_set_level(M2_PIN_IN1, 1);
    gpio_set_level(M2_PIN_IN2, 0);
}

void motor_right(void) {
    gpio_set_level(M1_PIN_IN1, 1);
    gpio_set_level(M1_PIN_IN2, 0);

    gpio_set_level(M2_PIN_IN1, 0);
    gpio_set_level(M2_PIN_IN2, 1);
}

void uart_init(void) {
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    uart_driver_install(UART_PORT, 1024, 0, 0, NULL, 0);

    uart_param_config(UART_PORT, &uart_config);

    uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN,UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

void uart_sent_command(char * command) {
    uart_write_bytes(UART_PORT, command, strlen(command));

    uart_write_bytes(UART_PORT, "\n", 1);
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

    listen(listen_sock, 1);

    while(1) {
        struct sockaddr_in client_addr;

        socklen_t client_addr_len = sizeof(client_addr);

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
