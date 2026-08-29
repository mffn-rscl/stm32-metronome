#include "../Inc/i2c_driver.h"

#include "../CMSIS/Device/stm32f103xb.h"
void i2c1_init()
{
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  
  GPIOB->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_MODE7 | GPIO_CRL_CNF6 | GPIO_CRL_CNF7);
  GPIOB->CRL |= (GPIOB_I2C1_MASK << GPIO_CRL_MODE6_Pos) | (GPIOB_I2C1_MASK << GPIO_CRL_MODE7_Pos);


  I2C1->CR1 &= ~I2C_CR1_PE;
  
  I2C1->CR2 = I2C1_APB1_FREQUENCY;
  I2C1->CCR = I2C1_APB1_FREQUENCY_DIVIDER;
  I2C1->TRISE = I2C1_TRISE_VAL;
  I2C1->CR2 |= I2C_CR2_DMAEN;

  I2C1->CR1 |= I2C_CR1_PE;
}

void i2c1_start()
{
  I2C1->CR1 |=I2C_CR1_START;
  while (!(I2C1->SR1 & I2C_SR1_SB));
}


void i2c1_slave_adress(uint8_t addr)
{
  I2C1->DR = (addr << 1);
  while(!(I2C1->SR1 & I2C_SR1_ADDR));

  (void)I2C1->SR1;
  (void)I2C1->SR2;
}


void i2c1_data(uint8_t data)
{
  I2C1->DR = data;
  while(!(I2C1->SR1 & I2C_SR1_TXE));
}

void i2c1_stop_condition()
{
  while(!(I2C1->SR1 & I2C_SR1_BTF));
  I2C1->CR1 |= I2C_CR1_STOP;
  
}

