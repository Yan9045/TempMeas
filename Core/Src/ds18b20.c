#include "ds18b20.h"
#include "tim.h"

void DS18B20_Init(void)
{
    // 配置DS18B20引脚
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DS18B20_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DS18B20_PORT, &GPIO_InitStruct);
    
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_SET);
}

uint8_t DS18B20_Reset(void)
{
    uint8_t presence;
    
    // 主机拉低总线480-960us
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
    Delay_us(480);
    
    // 释放总线，等待15-60us
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_SET);
    Delay_us(60);
    
    // 检测DS18B20响应
    presence = HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN);
    
    // 等待DS18B20释放总线
    Delay_us(420);
    
    return presence;
}

void DS18B20_WriteBit(uint8_t bit)
{
    if (bit)
    {
        // 写1时序
        HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
        Delay_us(6);
        HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_SET);
        Delay_us(64);
    }
    else
    {
        // 写0时序
        HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
        Delay_us(60);
        HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_SET);
        Delay_us(10);
    }
}

void DS18B20_WriteByte(uint8_t byte)
{
    uint8_t i;
    
    for (i = 0; i < 8; i++)
    {
        DS18B20_WriteBit(byte & 0x01);
        byte >>= 1;
    }
}

uint8_t DS18B20_ReadBit(void)
{
    uint8_t bit = 0;
    
    // 主机拉低总线至少1us
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
    Delay_us(2);
    
    // 释放总线，等待15us内读取数据
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_SET);
    Delay_us(10);
    
    // 读取数据
    bit = HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN);
    
    // 等待时序结束
    Delay_us(50);
    
    return bit;
}

uint8_t DS18B20_ReadByte(void)
{
    uint8_t i, byte = 0;
    
    for (i = 0; i < 8; i++)
    {
        byte >>= 1;
        if (DS18B20_ReadBit())
            byte |= 0x80;
    }
    
    return byte;
}

float DS18B20_ReadTemperature(void)
{
    uint8_t temp_l, temp_h;
    int16_t temp;
    float temperature;
    
    // 复位DS18B20
    DS18B20_Reset();
    
    // 发送跳过ROM命令
    DS18B20_WriteByte(0xCC);
    
    // 发送温度转换命令
    DS18B20_WriteByte(0x44);
    
    // 等待转换完成
    while (!DS18B20_ReadBit());
    
    // 复位DS18B20
    DS18B20_Reset();
    
    // 发送跳过ROM命令
    DS18B20_WriteByte(0xCC);
    
    // 发送读取温度命令
    DS18B20_WriteByte(0xBE);
    
    // 读取温度数据
    temp_l = DS18B20_ReadByte();
    temp_h = DS18B20_ReadByte();
    
    // 组合温度数据
    temp = (temp_h << 8) | temp_l;
    
    // 转换为实际温度值
    temperature = (float)temp / 16.0;
    
    return temperature;
}