#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>

#define SENSOR_POWER_GPIO_NODE DT_NODELABEL(gpioa) // Pino PA04
#define SENSOR_POWER_PIN 4

#define I2C_NODE DT_NODELABEL(i2c0)

static const struct device *gpio_dev;
static const struct device *i2c_dev;

void enable_si7021(void)
{
    gpio_dev = DEVICE_DT_GET(SENSOR_POWER_GPIO_NODE);
    if (!device_is_ready(gpio_dev)) {
        printk("Erro: GPIO não está pronto!\n");
        return;
    }

    gpio_pin_configure(gpio_dev, SENSOR_POWER_PIN, GPIO_OUTPUT);
    gpio_pin_set(gpio_dev, SENSOR_POWER_PIN, 1); // Liga o sensor
}

void main(void)
{
    printk("Inicializando Si7021...\n");

    enable_si7021();

    i2c_dev = DEVICE_DT_GET(I2C_NODE);
    if (!device_is_ready(i2c_dev)) {
        printk("Erro: I2C não está pronto!\n");
        return;
    }

    uint8_t cmd = 0xE3;  // Comando para medir temperatura (Hold Master Mode)
    uint8_t data[2];

    int ret = i2c_write_read(i2c_dev, 0x40, &cmd, 1, data, 2);
    if (ret < 0) {
        printk("Erro ao ler o Si7021!\n");
        return;
    }

    uint16_t raw_temp = (data[0] << 8) | data[1];
    float temp_c = ((175.72 * raw_temp) / 65536.0) - 46.85;

    printk("Temperatura: %.2f°C\n", temp_c);
}
