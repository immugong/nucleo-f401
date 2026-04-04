/*******************************************************************************
* Copyright (c) 2020-2023, STMicroelectronics - All Rights Reserved
*
* This file is part of the VL53L7CX/VL53L8CX Ultra Lite Driver and is dual licensed,
* either 'STMicroelectronics Proprietary license'
* or 'BSD 3-clause "New" or "Revised" License' , at your option.
*
* VL53L7CX: Copyright 2020 STMicroelectronics
* VL53L8CX: Copyright 2023 STMicroelectronics
*******************************************************************************/
#include "platform.h"
#include "main.h"
#include "i2c.h"

// 两款传感器共用I2C1外设，统一声明避免重复
extern I2C_HandleTypeDef 	hi2c1;

/************************** VL53L7CX 底层函数实现 **************************/
uint8_t VL53L7CX_RdByte(
		VL53L7CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_value)
{
	uint8_t status = 0;
	uint8_t data_write[2];
	uint8_t data_read[1];
	data_write[0] = (RegisterAdress >> 8) & 0xFF;
	data_write[1] = RegisterAdress & 0xFF;
	status = HAL_I2C_Master_Transmit(&hi2c1, p_platform->address, data_write, 2, 100);
	status = HAL_I2C_Master_Receive(&hi2c1, p_platform->address, data_read, 1, 100);
	*p_value = data_read[0];
  
	return status;
}

uint8_t VL53L7CX_WrByte(
		VL53L7CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t value)
{
	uint8_t data_write[3];
	uint8_t status = 0;
	data_write[0] = (RegisterAdress >> 8) & 0xFF;
	data_write[1] = RegisterAdress & 0xFF;
	data_write[2] = value & 0xFF;
	status = HAL_I2C_Master_Transmit(&hi2c1,p_platform->address, data_write, 3, 100);
	return status;
}

uint8_t VL53L7CX_WrMulti(
		VL53L7CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_values,
		uint32_t size)
{
	uint8_t status = HAL_I2C_Mem_Write(&hi2c1, p_platform->address, RegisterAdress,
									I2C_MEMADD_SIZE_16BIT, p_values, size, 65535);
	return status;
}

uint8_t VL53L7CX_RdMulti(
		VL53L7CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_values,
		uint32_t size)
{
	uint8_t status;
	uint8_t data_write[2];
	data_write[0] = (RegisterAdress>>8) & 0xFF;
	data_write[1] = RegisterAdress & 0xFF;
	status = HAL_I2C_Master_Transmit(&hi2c1, p_platform->address, data_write, 2, 100);
	status += HAL_I2C_Master_Receive(&hi2c1, p_platform->address, p_values, size, 100);
	return status;
}

uint8_t VL53L7CX_Reset_Sensor(VL53L7CX_Platform *p_platform)
{
	/* 通用复位模板，用户可根据硬件修改引脚操作 */
	/* Set pin LPN/AVDD/VDDIO to LOW */
	VL53L7CX_WaitMs(p_platform, 100);
	/* Set pin LPN/AVDD/VDDIO to HIGH */
	VL53L7CX_WaitMs(p_platform, 100);
  
	return 0;
}

void VL53L7CX_SwapBuffer(
		uint8_t 		*buffer,
		uint16_t 	 	 size)
{
	uint32_t i, tmp;
	/* 缓冲区字节交换，4字节为单位（API强制要求） */
	for(i = 0; i < size; i = i + 4)
	{
		tmp = (buffer[i]<<24) | (buffer[i+1]<<16) | (buffer[i+2]<<8) | (buffer[i+3]);
		memcpy(&(buffer[i]), &tmp, 4);
	}
}

uint8_t VL53L7CX_WaitMs(
		VL53L7CX_Platform *p_platform,
               uint32_t TimeMs)
{
	HAL_Delay(TimeMs);
	return 0;
}

/************************** VL53L8CX 底层函数实现 **************************/
uint8_t VL53L8CX_RdByte(
		VL53L8CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_value)
{
	uint8_t status = 0;
	uint8_t data_write[2];
	uint8_t data_read[1];
	data_write[0] = (RegisterAdress >> 8) & 0xFF;
	data_write[1] = RegisterAdress & 0xFF;
	status = HAL_I2C_Master_Transmit(&hi2c1, p_platform->address, data_write, 2, 100);
	status = HAL_I2C_Master_Receive(&hi2c1, p_platform->address, data_read, 1, 100);
	*p_value = data_read[0];
  
	return status;
}

uint8_t VL53L8CX_WrByte(
		VL53L8CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t value)
{
	uint8_t data_write[3];
	uint8_t status = 0;
	data_write[0] = (RegisterAdress >> 8) & 0xFF;
	data_write[1] = RegisterAdress & 0xFF;
	data_write[2] = value & 0xFF;
	status = HAL_I2C_Master_Transmit(&hi2c1,p_platform->address, data_write, 3, 100);
	return status;
}

uint8_t VL53L8CX_WrMulti(
		VL53L8CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_values,
		uint32_t size)
{
	uint8_t status = HAL_I2C_Mem_Write(&hi2c1, p_platform->address, RegisterAdress,
									I2C_MEMADD_SIZE_16BIT, p_values, size, 65535);
	return status;
}

uint8_t VL53L8CX_RdMulti(
		VL53L8CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_values,
		uint32_t size)
{
	uint8_t status;
	uint8_t data_write[2];
	data_write[0] = (RegisterAdress>>8) & 0xFF;
	data_write[1] = RegisterAdress & 0xFF;
	status = HAL_I2C_Master_Transmit(&hi2c1, p_platform->address, data_write, 2, 100);
	status += HAL_I2C_Master_Receive(&hi2c1, p_platform->address, p_values, size, 100);
	return status;
}

uint8_t VL53L8CX_Reset_Sensor(VL53L8CX_Platform *p_platform)
{
	/* 硬件实际复位逻辑：操作LPn/PWR_EN引脚（左/中/右三路） */
	
	//HAL_GPIO_WritePin(LPn_C_GPIO_Port, LPn_C_Pin, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(PWR_EN_C_GPIO_Port, PWR_EN_C_Pin, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(LPn_R_GPIO_Port, LPn_R_Pin, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(PWR_EN_R_GPIO_Port, PWR_EN_R_Pin, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(LPn_L_GPIO_Port, LPn_L_Pin, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(PWR_EN_L_GPIO_Port, PWR_EN_L_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	
	//HAL_GPIO_WritePin(PWR_EN_C_GPIO_Port, PWR_EN_C_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	//HAL_GPIO_WritePin(LPn_C_GPIO_Port, LPn_C_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	
	return 0;
}

void VL53L8CX_SwapBuffer(
		uint8_t 		*buffer,
		uint16_t 	 	 size)
{
	uint32_t i, tmp;
	/* 缓冲区字节交换，与L7实现一致，4字节为单位 */
	for(i = 0; i < size; i = i + 4)
	{
		tmp = (buffer[i]<<24) | (buffer[i+1]<<16) | (buffer[i+2]<<8) | (buffer[i+3]);
		memcpy(&(buffer[i]), &tmp, 4);
	}
}

uint8_t VL53L8CX_WaitMs(
		VL53L8CX_Platform *p_platform,
               uint32_t TimeMs)
{
	HAL_Delay(TimeMs);
	return 0;
}