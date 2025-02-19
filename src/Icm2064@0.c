#include <Icm2064@0.h>
#include <error.h>


static const struct device *dev = device_get_binding("Icm20648@0");

error_t init_icm2064(){

}

error_t read_icm2064(uint8_t *data, uint8_t len){

}

error_t getAceccelX(uint8_t *data){

}

error_t getAceccelY(uint8_t *data){

}

error_t getAceccelZ(uint8_t *data){

}

error_t getAceccel(sensor_data_t data){

}

error_t getGyroX(uint8_t *data){

}

error_t getGyroY(uint8_t *data){

}

error_t getGyroZ(uint8_t *data){

}

error_t getGyro(sensor_data_t data){

}