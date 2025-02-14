#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <stdio.h>
#include <string.h>

/* Defina os nomes dos dispositivos UART conforme definido no DTS */
#define UART0_DEVICE_NAME "UART_0"  // Nome do dispositivo UART0 no DTS
#define UART2_DEVICE_NAME "UART_2"  // Nome do dispositivo UART2 no DTS

/* Ponteiros para os dispositivos UART */
const struct device *uart0;
const struct device *uart2;

/* Configuração UART */
struct uart_config uart_cfg = {
    .baudrate = 115200,
    .parity = UART_CFG_PARITY_NONE,
    .stop_bits = UART_CFG_STOP_BITS_1,
    .flow_ctrl = UART_CFG_FLOW_CTRL_NONE,
    .data_bits = UART_CFG_DATA_BITS_8,
};

/* Função para enviar uma string via UART */
void send_str(const struct device *uart, char *str)
{
    int msg_len = strlen(str);

    for (int i = 0; i < msg_len; i++) {
        uart_poll_out(uart, str[i]);
    }

    printk("Device %s sent: \"%s\"\n", uart->name, str);
}

/* Função para receber uma string via UART */
void recv_str(const struct device *uart, char *str)
{
    char *head = str;
    char c;

    while (!uart_poll_in(uart, &c)) {
        *head++ = c;
    }
    *head = '\0';

    printk("Device %s received: \"%s\"\n", uart->name, str);
}

int main(void)
{
    int rc;
    char send_buf[64];
    char recv_buf[64];
    int i = 10;

    /* Obtém os dispositivos UART pelo nome */
    uart0 = device_get_binding(UART0_DEVICE_NAME);
    uart2 = device_get_binding(UART2_DEVICE_NAME);

    /* Verifica se os dispositivos UART estão prontos */
    if (!uart0) {
        printk("UART0 device not found\n");
        return -1;
    }
    if (!uart2) {
        printk("UART2 device not found\n");
        return -1;
    }

    /* Configura os dispositivos UART */
    rc = uart_configure(uart0, &uart_cfg);
    if (rc) {
        printk("Could not configure device %s\n", uart0->name);
    }
    rc = uart_configure(uart2, &uart_cfg);
    if (rc) {
        printk("Could not configure device %s\n", uart2->name);
    }

    /* Loop principal */
    while (i--) {
        snprintf(send_buf, 64, "Hello from device %s, num %d", uart0->name, i);
        send_str(uart0, send_buf);
        /* Espera um tempo para a mensagem chegar ao segundo UART */
        k_sleep(K_MSEC(100));
        recv_str(uart2, recv_buf);

        k_sleep(K_MSEC(1000));
    }

    /* Altera a taxa de transmissão (baudrate) */
    uart_cfg.baudrate = 9600;
    printk("\nChanging baudrate of both uart devices to %d!\n\n", uart_cfg.baudrate);

    rc = uart_configure(uart0, &uart_cfg);
    if (rc) {
        printk("Could not configure device %s\n", uart0->name);
    }
    rc = uart_configure(uart2, &uart_cfg);
    if (rc) {
        printk("Could not configure device %s\n", uart2->name);
    }

    i = 10;
    while (i--) {
        snprintf(send_buf, 64, "Hello from device %s, num %d", uart0->name, i);
        send_str(uart0, send_buf);
        /* Espera um tempo para a mensagem chegar ao segundo UART */
        k_sleep(K_MSEC(100));
        recv_str(uart2, recv_buf);

        k_sleep(K_MSEC(1000));
    }

    return 0;
}