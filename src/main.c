#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <stdio.h>
#include <string.h>
#define MSG_LEN 128

/* Nome do dispositivo UART */
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)

static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

static char tx_buff[MSG_LEN];
static int  tx_buff_pos;

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

void print_uart(char *buf)
{
	int msg_len = strlen(buf);

	for (int i = 0; i < msg_len; i++) {
		uart_poll_out(uart_dev, buf[i]);
	}
}


void uart_tx_cb(const struct device *dev, void *user_data)
{
	uint8_t c;
    tx_buff_pos = 0;

	if (!uart_irq_update(uart_dev)) {
		return;
	}

	if (!uart_irq_rx_ready(uart_dev)) {
		return;
	}

    memset(tx_buff, 0, MSG_LEN);

	
	while (uart_fifo_read(uart_dev, &c, 1) == 1) {
		if (c == '\n'  && tx_buff_pos > 0) {
			tx_buff[tx_buff_pos] = '\0';
			tx_buff_pos = 0;
		} else if (tx_buff_pos < (sizeof(tx_buff) - 1)) {
			tx_buff[tx_buff_pos++] = c;
		}
	}

    print_uart("Dados sendo transmitindo:\n");
    print_uart(tx_buff);
}

int main(void)
{
    int rc;

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

    rc = uart_irq_callback_user_data_set(uart_dev, uart_tx_cb, NULL);
    if (rc < 0) {
		if (rc == -ENOTSUP) {
			printk("Interrupt-driven UART API support not enabled\n");
		} else if (rc == -ENOSYS) {
			printk("UART device does not support interrupt-driven API\n");
		} else {
			printk("Error setting UART callback: %d\n", rc);
		}
		return 0;
	}
    uart_irq_rx_enable(uart_dev);
    print_uart("Hello, I'm João\n\n");

    /* Loop principal */
    while (1) {
        k_sleep(K_SECONDS(1));
    }

    return 0;
}