#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>

#define UART_BUF_SIZE 64

K_MSGQ_DEFINE(uart_msgq, UART_BUF_SIZE, 10, 4);

#define STACK_SIZE 1024

//#define UART_DEVICE_NODE DT_NODELABEL(zephyr_shell_uart)  
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)
const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

uint8_t rx_buf[UART_BUF_SIZE];
uint8_t tx_buf[UART_BUF_SIZE];

int rx_buf_pos = 0;

// void uart_cb(const struct device *dev, void *user_data) {
//     uint8_t c;
    
//     while (uart_fifo_read(dev, &c, 1) > 0) {
//         if (c == '\n') {  
//             rx_buf[rx_buf_pos] = '\0';
//             rx_buf_pos = 0;  
//             memset(rx_buf, 0, UART_BUF_SIZE);
//         } else {
//             if (rx_buf_pos < UART_BUF_SIZE - 1) {
//                 rx_buf[rx_buf_pos++] = c;
//             }
//         }
//     }
// }

void rx_task(void *p1, void *p2, void *p3){
    char rx_buffer[UART_BUF_SIZE] = {0};

    while(true){
        if(uart_poll_in(uart_dev, rx_buffer) != 0){
            k_sleep(K_MSEC(100));
            continue;
        }

        printk("Recebido: %s\n", rx_buffer);
        
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
    char data[UART_BUF_SIZE] = "Temperatura: 25.5°C\n\0";
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

    while(1){
        k_sleep(K_MSEC(2000));
    }

};

//K_THREAD_DEFINE(tx_task, STACK_SIZE, uart_tx_data, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(rx_read, STACK_SIZE, rx_task, NULL, NULL, NULL, 5, 0, 0);