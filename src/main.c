#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <stdio.h>
#include <string.h>

/* Nome do dispositivo UART */
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)

static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

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

    /* Obtém o dispositivo UART pelo nome */
    //art_dev = device_get_binding(UART_DEVICE_NAME);

    /* Verifica se o dispositivo UART está pronto */
    if (!uart_dev) {
        printk("UART device not found\n");
        return -1;
    }

    /* Configura o dispositivo UART */
    rc = uart_configure(uart_dev, &uart_cfg);
    if (rc) {
        printk("Could not configure device %s\n", uart_dev->name);
    }

    /* Loop principal */
    while (i--) {
        snprintf(send_buf, 64, "Hello from device %s, num %d", uart_dev->name, i);
        send_str(uart_dev, send_buf);
        /* Espera um tempo para a mensagem chegar ao outro dispositivo */
        k_sleep(K_MSEC(100));
        recv_str(uart_dev, recv_buf);

        k_sleep(K_MSEC(1000));
    }

    /* Altera a taxa de transmissão (baudrate) */
    uart_cfg.baudrate = 9600;
    printk("\nChanging baudrate of the UART device to %d!\n\n", uart_cfg.baudrate);

    rc = uart_configure(uart_dev, &uart_cfg);
    if (rc) {
        printk("Could not configure device %s\n", uart_dev->name);
    }

    i = 10;
    while (i--) {
        snprintf(send_buf, 64, "Hello from device %s, num %d", uart_dev->name, i);
        send_str(uart_dev, send_buf);
        /* Espera um tempo para a mensagem chegar ao outro dispositivo */
        k_sleep(K_MSEC(100));
        recv_str(uart_dev, recv_buf);

        k_sleep(K_MSEC(1000));
    }

    return 0;
}