#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>

#define UART_BUF_SIZE 64

K_MSGQ_DEFINE(uart_msgq, UART_BUF_SIZE, 10, 4);

#define STACK_SIZE 1024
K_THREAD_STACK_DEFINE(thread_stack, STACK_SIZE);
struct k_thread tx_thread_data;
k_tid_t tx_thread_tid;

//#define UART_DEVICE_NODE DT_NODELABEL(zephyr_shell_uart)  
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)
const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

uint8_t rx_buf[UART_BUF_SIZE];
uint8_t tx_buf[UART_BUF_SIZE];

int rx_buf_pos = 0;

void uart_cb(const struct device *dev, void *user_data) {
    uint8_t c;
    
    while (uart_fifo_read(dev, &c, 1) > 0) {
        if (c == '\n') {  
            rx_buf[rx_buf_pos] = '\0';
            printk("Recebido: %s\n", rx_buf);
            rx_buf_pos = 0;  
            memset(rx_buf, 0, UART_BUF_SIZE);
        } else {
            if (rx_buf_pos < UART_BUF_SIZE - 1) {
                rx_buf[rx_buf_pos++] = c;
            }
        }
    }
}

void send_by_uart(char *buf)
{
	int msg_len = strlen(buf);

	for (int i = 0; i < msg_len; i++) {
		uart_poll_out(uart_dev, buf[i]);
	}
}

void uart_tx_data(const struct device *dev, void *p1, void *p2){
   while(1){
        if(k_msgq_get(&uart_msgq, &tx_buf, K_FOREVER) == 0){
            send_by_uart(&tx_buf);
            memset(tx_buf, 0, UART_BUF_SIZE);
        }
   };
}

void collect_sensor_data(){
    char data[UART_BUF_SIZE];
    while(1){
        // JOTA - Coleta os dados do sensor

        // Coloca os dados na fila
        k_msgq_put(&uart_msgq, data, K_FOREVER);
        break;
    }
}

void main(void) {
    if (!device_is_ready(uart_dev)) {
        printk("UART não está pronta!\n");
        return;
    }

    // Configura a UART para usar interrupção no RX
    uart_irq_callback_user_data_set(uart_dev, uart_cb, NULL);
    uart_irq_rx_enable(uart_dev);

    tx_thread_tid = k_thread_create(&tx_thread_data, thread_stack, STACK_SIZE,
                                    uart_tx_data, uart_dev, NULL, NULL,
                                    1, 0, K_NO_WAIT);

    while(1){
        collect_sensor_data();
        K_
    }

}
