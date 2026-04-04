/**
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef _PLATFORM_H_
#define _PLATFORM_H_
#pragma once
#include <stdint.h>
#include <string.h>
#include "stm32f4xx.h"

/**
 * @brief Structure VL53L7CX_Platform needs to be filled by the customer,
 * depending on his platform. At least, it contains the VL53L7CX I2C address.
 * Some additional fields can be added, as descriptors, or platform
 * dependencies. Anything added into this structure is visible into the platform
 * layer.
 */
typedef struct
{
	/* To be filled with customer's platform. At least an I2C address/descriptor
	 * needs to be added */
	/* Example for most standard platform : I2C address of sensor */
    uint16_t  			address;
} VL53L7CX_Platform;

/**
 * @brief Structure VL53L8CX_Platform needs to be filled by the customer,
 * depending on his platform. At least, it contains the VL53L8CX I2C address.
 * Some additional fields can be added, as descriptors, or platform
 * dependencies. Anything added into this structure is visible into the platform
 * layer.
 * @note 与VL53L7CX_Platform格式完全一致，保证平台适配统一性
 */
typedef struct
{
	/* To be filled with customer's platform. At least an I2C address/descriptor
	 * needs to be added */
	/* Example for most standard platform : I2C address of sensor */
    uint16_t  			address;
} VL53L8CX_Platform;

/*
 * @brief VL53L7CX 宏定义：单区域目标数（I2C传输调优）
 * This value can be changed by user, in order to tune I2C
 * transaction, and also the total memory size (a lower number of target per
 * zone means a lower RAM). The value must be between 1 and 4.
 */
#define 	VL53L7CX_NB_TARGET_PER_ZONE		1U

/*
 * @brief VL53L8CX 宏定义：单区域目标数（I2C传输调优）
 * 与L7保持一致默认值，取值范围1~4，用户可独立修改
 */
#define 	VL53L8CX_NB_TARGET_PER_ZONE		1U

/*
 * @brief VL53L7CX 可选宏：关闭固件与用户数据的转换，使用原始格式提升精度
 * By default there is a conversion between firmware and user data. Using this macro
 * allows to use the firmware format instead of user format. The firmware format allows
 * an increased precision.
 */
// #define 	VL53L7CX_USE_RAW_FORMAT

/*
 * @brief VL53L8CX 可选宏：关闭固件与用户数据的转换，使用原始格式提升精度
 * 与L7逻辑一致，独立宏定义，可单独使能/关闭
 */
// #define 	VL53L8CX_USE_RAW_FORMAT

/*
 * @brief VL53L7CX 输出配置宏：禁用指定输出以减少I2C访问（用户可选定义）
 * User can define some macros if he wants to disable selected output, in order to reduce
 * I2C access.
 */
// #define VL53L7CX_DISABLE_AMBIENT_PER_SPAD
// #define VL53L7CX_DISABLE_NB_SPADS_ENABLED
// #define VL53L7CX_DISABLE_NB_TARGET_DETECTED
// #define VL53L7CX_DISABLE_SIGNAL_PER_SPAD
// #define VL53L7CX_DISABLE_RANGE_SIGMA_MM
// #define VL53L7CX_DISABLE_DISTANCE_MM
// #define VL53L7CX_DISABLE_REFLECTANCE_PERCENT
// #define VL53L7CX_DISABLE_TARGET_STATUS
// #define VL53L7CX_DISABLE_MOTION_INDICATOR

/*
 * @brief VL53L8CX 输出配置宏：禁用指定输出以减少I2C访问（用户可选定义）
 * 与L7输出项完全一致，独立宏定义，可单独禁用指定输出
 */
// #define VL53L8CX_DISABLE_AMBIENT_PER_SPAD
// #define VL53L8CX_DISABLE_NB_SPADS_ENABLED
// #define VL53L8CX_DISABLE_NB_TARGET_DETECTED
// #define VL53L8CX_DISABLE_SIGNAL_PER_SPAD
// #define VL53L8CX_DISABLE_RANGE_SIGMA_MM
// #define VL53L8CX_DISABLE_DISTANCE_MM
// #define VL53L8CX_DISABLE_REFLECTANCE_PERCENT
// #define VL53L8CX_DISABLE_TARGET_STATUS
// #define VL53L8CX_DISABLE_MOTION_INDICATOR

/************************** VL53L7CX 平台底层函数声明 **************************/
/**
 * @brief 读单个字节（VL53L7CX专用）
 * @param (VL53L7CX_Platform*) p_platform : 平台结构体指针
 * @param (uint16_t) RegisterAdress : 待读I2C寄存器地址
 * @param (uint8_t) *p_value : 读取数据缓冲区指针
 * @return (uint8_t) status : 0 if OK
 */
uint8_t VL53L7CX_RdByte(
		VL53L7CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_value);

/**
 * @brief 写单个字节（VL53L7CX专用，必实现）
 * @param (VL53L7CX_Platform*) p_platform : 平台结构体指针
 * @param (uint16_t) RegisterAdress : 待写I2C寄存器地址
 * @param (uint8_t) value : 待写入数据
 * @return (uint8_t) status : 0 if OK
 */
uint8_t VL53L7CX_WrByte(
		VL53L7CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t value);

/**
 * @brief 读多个字节（VL53L7CX专用，必实现）
 * @param (VL53L7CX_Platform*) p_platform : 平台结构体指针
 * @param (uint16_t) RegisterAdress : 待读I2C寄存器起始地址
 * @param (uint8_t) *p_values : 读取数据缓冲区指针
 * @param (uint32_t) size : 读取数据长度
 * @return (uint8_t) status : 0 if OK
 */
uint8_t VL53L7CX_RdMulti(
		VL53L7CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_values,
		uint32_t size);

/**
 * @brief 写多个字节（VL53L7CX专用，必实现）
 * @param (VL53L7CX_Platform*) p_platform : 平台结构体指针
 * @param (uint16_t) RegisterAdress : 待写I2C寄存器起始地址
 * @param (uint8_t) *p_values : 待写入数据缓冲区指针
 * @param (uint32_t) size : 待写入数据长度
 * @return (uint8_t) status : 0 if OK
 */
uint8_t VL53L7CX_WrMulti(
		VL53L7CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_values,
		uint32_t size);

/**
 * @brief 传感器硬件复位（VL53L7CX专用，可选实现）
 * @note  API内部未使用，用户可根据需求实现硬件复位逻辑
 * @param (VL53L7CX_Platform*) p_platform : 平台结构体指针
 * @return (uint8_t) status : 0 if OK
 */
uint8_t VL53L7CX_Reset_Sensor(
		VL53L7CX_Platform *p_platform);

/**
 * @brief 缓冲区字节交换（VL53L7CX专用，必实现）
 * @note  缓冲区大小始终为4的倍数（4,8,12,16...）
 * @param (uint8_t*) buffer : 待交换缓冲区指针
 * @param (uint16_t) size : 缓冲区大小
 */
void VL53L7CX_SwapBuffer(
		uint8_t 		*buffer,
		uint16_t 	 	 size);

/**
 * @brief 毫秒级延时（VL53L7CX专用，必实现）
 * @note  API内部大量使用，必须实现精准延时
 * @param (VL53L7CX_Platform*) p_platform : 平台结构体指针
 * @param (uint32_t) TimeMs : 延时时间(ms)
 * @return (uint8_t) status : 0 if wait is finished
 */
uint8_t VL53L7CX_WaitMs(
		VL53L7CX_Platform *p_platform,
		uint32_t TimeMs);

/************************** VL53L8CX 平台底层函数声明 **************************/
/**
 * @brief 读单个字节（VL53L8CX专用）
 * @param (VL53L8CX_Platform*) p_platform : 平台结构体指针
 * @param (uint16_t) RegisterAdress : 待读I2C寄存器地址
 * @param (uint8_t) *p_value : 读取数据缓冲区指针
 * @return (uint8_t) status : 0 if OK
 */
uint8_t VL53L8CX_RdByte(
		VL53L8CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_value);

/**
 * @brief 写单个字节（VL53L8CX专用，必实现）
 * @param (VL53L8CX_Platform*) p_platform : 平台结构体指针
 * @param (uint16_t) RegisterAdress : 待写I2C寄存器地址
 * @param (uint8_t) value : 待写入数据
 * @return (uint8_t) status : 0 if OK
 */
uint8_t VL53L8CX_WrByte(
		VL53L8CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t value);

/**
 * @brief 读多个字节（VL53L8CX专用，必实现）
 * @param (VL53L8CX_Platform*) p_platform : 平台结构体指针
 * @param (uint16_t) RegisterAdress : 待读I2C寄存器起始地址
 * @param (uint8_t) *p_values : 读取数据缓冲区指针
 * @param (uint32_t) size : 读取数据长度
 * @return (uint8_t) status : 0 if OK
 */
uint8_t VL53L8CX_RdMulti(
		VL53L8CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_values,
		uint32_t size);

/**
 * @brief 写多个字节（VL53L8CX专用，必实现）
 * @param (VL53L8CX_Platform*) p_platform : 平台结构体指针
 * @param (uint16_t) RegisterAdress : 待写I2C寄存器起始地址
 * @param (uint8_t) *p_values : 待写入数据缓冲区指针
 * @param (uint32_t) size : 待写入数据长度
 * @return (uint8_t) status : 0 if OK
 */
uint8_t VL53L8CX_WrMulti(
		VL53L8CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_values,
		uint32_t size);

/**
 * @brief 传感器硬件复位（VL53L8CX专用，可选实现）
 * @note  API内部未使用，用户可根据需求实现硬件复位逻辑
 * @param (VL53L8CX_Platform*) p_platform : 平台结构体指针
 * @return (uint8_t) status : 0 if OK
 */
uint8_t VL53L8CX_Reset_Sensor(
		VL53L8CX_Platform *p_platform);

/**
 * @brief 缓冲区字节交换（VL53L8CX专用，必实现）
 * @note  缓冲区大小始终为4的倍数（4,8,12,16...）
 * @param (uint8_t*) buffer : 待交换缓冲区指针
 * @param (uint16_t) size : 缓冲区大小
 */
void VL53L8CX_SwapBuffer(
		uint8_t 		*buffer,
		uint16_t 	 	 size);

/**
 * @brief 毫秒级延时（VL53L8CX专用，必实现）
 * @note  API内部大量使用，必须实现精准延时
 * @param (VL53L8CX_Platform*) p_platform : 平台结构体指针
 * @param (uint32_t) TimeMs : 延时时间(ms)
 * @return (uint8_t) status : 0 if wait is finished
 */
uint8_t VL53L8CX_WaitMs(
		VL53L8CX_Platform *p_platform,
		uint32_t TimeMs);

#endif	// _PLATFORM_H_