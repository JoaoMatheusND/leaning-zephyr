#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#define MSG_LEN 32
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)

K_MSGQ_DEFINE(pc_msg, MSG_LEN, 10, 1);
K_MSGQ_DEFINE(aceleormetro_msg, MSG_LEN, 10, 1);

static const struct device const *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

static char rx_buff[MSG_LEN];
static char tx_buff[MSG_LEN];

static int rx_buffer_pos;
static int tx_buffer_pos;

void serial_cb(const struct device *dev, void *user_data){
    uint8_t data;

    if(!uart_irq_update(uart_dev) || !uart_irq_rx_ready(uart_dev)){
        return;
    }

    while(uart_fifo_read(uart_dev, &data, 1)){
        if((data == '\n' || data == '\r') && rx_buffer_pos > 0){
            rx_buff[rx_buffer_pos] = '\0';

            k_msgq_put(&pc_msg, rx_buff, K_NO_WAIT);

            rx_buffer_pos = 0;
        }else{
            rx_buff[rx_buffer_pos++] = data;
        }
    }
}

void print_uart(const char *buff){
    int len = strlen(buff);

    for(int i = 0; i<len; i++){
        uart_poll_out(uart_dev, buff[i]);
    }
}

void rx_task(void){
    while(1){
        if(k_msgq_get(&pc_msg, &rx_buff, K_FOREVER) == 0){
            printk("Receiver data %s\n", rx_buff);
        }
    }
}

void tx_task(void){
    while(1){
        if(k_msgq_get(&pc_msg, &tx_buff, K_FOREVER) == 0){
            printk("Transmitting data: %s\n", tx_buff);
            print_uart(tx_buff);
        }
    }
}

K_THREAD_DEFINE(rx_task_id, 1024, rx_task, NULL, NULL, NULL, 7, 0, 0);
K_THREAD_DEFINE(tx_task_id, 1024, tx_task, NULL, NULL, NULL, 7, 0, 0);

void data_acelerometor(const char *buff){
    k_msgq_put(&aceleormetro_msg, buff, K_NO_WAIT);
}

int main(){
    char tx_buff[MSG_LEN];

    if(!device_is_ready(uart_dev)){
        printk("UART device not ready\n");
        return -1;
    }

    int ret = uart_irq_callback_user_data_set(uart_dev, serial_cb, NULL);

    if(ret < 0){
        if (ret == -ENOTSUP) {
			printk("Interrupt-driven UART API support not enabled\n");
		} else if (ret == -ENOSYS) {
			printk("UART device does not support interrupt-driven API\n");
		} else {
			printk("Error setting UART callback: %d\n", ret);
		}
		return 0;
    }

    uart_irq_rx_enable(uart_dev);


    return 0;
}