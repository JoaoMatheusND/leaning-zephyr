#include "zephyr/kernel.h"
#include "zephyr/drivers/gpio.h"
#include "zephyr/drivers/sensor/"
#include "zephyr/device.h"
#include "zephyr/types.h"

#define ICM2064_ADDR 0x0


typedef struct{
    uint8_t x;
    uint8_t y;
    uint8_t z;
} sensor_data_t;

/*  Inicializa o sensor */
error_t init_icm2064();

/*****************************************************/

//ACELEROMETO

/*  Pega o valor do vetor X do acelerometro, retorna um erro caso contrário*/
error_t getAceccelX(uint8_t *data);

/*  Pega o valor do vetor Y do acelerometro*, retorna um erro caso contrário/
error_t getAceccelY(uint8_t *data);

/*  Pega o valor do vetor Z do acelerometro, retorna um erro caso contrário*/
error_t getAceccelZ(uint8_t *data);

/*  Pega o um vetor do tipo [X, Y, Z] representando as coordenadas do acelerometro*/
error_t  getAceccel(sensor_data_t data);


/*****************************************************/

//GIROSCOPIO

/*  Pega o valor do vetor X do giroscopio, retorna um erro caso contrário*/
error_t getGyroX(uint_8 *data);

/*  Pega o valor do vetor Y do giroscopio, retorna um erro caso contrário*/
error_t getGyroY(uint_8 *data);

/*  Pega o valor do vetor Z do giroscopio, retorna um erro caso contrário*/
error_t getGyroZ(uint_8 *data);

/*  Pega o um vetor do tipo [X, Y, Z] representando as coordenadas do giroscopio*/
error_t  getGyro(sensor_data_t data);
