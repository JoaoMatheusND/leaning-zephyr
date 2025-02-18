#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>

#define UART_DEVICE_NODE DT_ALIAS(uart0)

static const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);
static uint8_t tx_buf[64];
static int tx_buf_len = 0;
static int tx_pos = 0;

// Callback de interrupção da UART
static void uart_cb(const struct device *dev, void *user_data) {
    ARG_UNUSED(user_data);

    if (uart_irq_update(dev)) {
        // Recebendo dados
        if (uart_irq_rx_ready(dev)) {
            uint8_t buf[64];
            int len = uart_fifo_read(dev, buf, sizeof(buf));
            if (len > 0) {
                buf[len] = '\0'; // Termina a string
                printk("Recebido: %s\n", buf);
            }
        }

        // Enviando dados
        if (uart_irq_tx_ready(dev) && tx_pos < tx_buf_len) {
            int len = uart_fifo_fill(dev, &tx_buf[tx_pos], tx_buf_len - tx_pos);
            tx_pos += len;
            if (tx_pos >= tx_buf_len) {
                uart_irq_tx_disable(dev); // Desabilita transmissão ao terminar
            }
        }
    }
}

// Função para enviar dados manualmente
void uart_send(const char *data) {
    tx_buf_len = strlen(data);
    memcpy(tx_buf, data, tx_buf_len);
    tx_pos = 0;
    uart_irq_tx_enable(uart_dev); // Inicia envio
}

void main(void) {
    if (!device_is_ready(uart_dev)) {
        printk("UART não está pronta!\n");
        return;
    }

    uart_irq_callback_user_data_set(uart_dev, uart_cb, NULL);
    uart_irq_rx_enable(uart_dev); // Habilita recepção

    while (1) {
        k_sleep(K_SECONDS(5));
        uart_send("Hello via UART Interrupt!\n");
    }
}
