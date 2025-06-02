#ifndef __DS18B20_H
#define __DS18B20_H

#include "main.h"

#define DS18B20_PORT GPIOB
#define DS18B20_PIN GPIO_PIN_3

void DS18B20_Init(void);
uint8_t DS18B20_Reset(void);
void DS18B20_WriteBit(uint8_t bit);
void DS18B20_WriteByte(uint8_t byte);
uint8_t DS18B20_ReadBit(void);
uint8_t DS18B20_ReadByte(void);
float DS18B20_ReadTemperature(void);

#endif