#ifndef I2C1_DRIVER_H
#define I2C1_DRIVER_H

#include <stdint.h>
#define GPIOB_I2C1_MASK 0xFU
#define I2C1_APB1_FREQUENCY 36U
#define I2C1_APB1_FREQUENCY_DIVIDER 180U
#define I2C1_TRISE_VAL 37U 

void i2c1_dma_enable(void);

void i2c1_init(void);

void i2c1_start(void);

void i2c1_slave_adress(uint8_t addr);

void i2c1_data(uint8_t data);

void i2c1_stop_condition(void);

#endif // I2C1_DRIVER_H
