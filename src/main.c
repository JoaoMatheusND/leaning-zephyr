#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>


#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)

const struct device *uart_dev;

#define BUF_SIZE 128
uint8_t rx_buf[BUF_SIZE]; // Buffer para armazenar os dados recebidos
uint8_t tx_msg[] = "Message from A!\n"; // Mensagem de envio do A

void uart_cb(const struct device *dev, void *user_data)
{
    uint8_t c;
    while (uart_poll_in(dev, &c) == 0) {
        printk("A received: %c", c); // Imprime os caracteres recebidos
    }
}

void send_message(void)
{
    for (int i = 0; i < strlen(tx_msg); i++) {
        uart_poll_out(uart_dev, tx_msg[i]);
    }
    printk("A sent: %s\n", tx_msg);
}

void main(void)
{
    uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

    if (!device_is_ready(uart_dev)) {
        printk("UART device not found!\n");
        return;
    }

    uart_irq_callback_user_data_set(uart_dev, uart_cb, NULL);
    uart_irq_rx_enable(uart_dev);

    while (1) {
        send_message();  // Envia a mensagem
        k_sleep(K_SECONDS(2));  // Espera por 2 segundos antes de enviar novamente

        // Agora aguarda uma resposta
        printk("A waiting for response...\n");
        k_sleep(K_SECONDS(2));  // Tempo de espera para resposta de B
    }
}