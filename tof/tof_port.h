#ifndef __TOF_PORT_H__
#define __TOF_PORT_H__

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************ 配置宏（一键切换传感器和采集模式） ************************/
// 核心区分宏：1=VL53L7CX，0=VL53L8CX
#define USE_VL53L7 0
// 采集模式：1=中断获取数据，0=轮询获取数据
#define is_interrupt 0

/************************ 对外接口 ************************/
/**
 * @brief  仅初始化TOF传感器（不创建任务）
 * @retval 0=成功，非0=失败（错误码与传感器驱动一致）
 */
uint8_t TOF_Init(void);

/**
 * @brief  仅创建并启动TOF测距任务
 * @note   必须先调用TOF_Init()初始化成功后才能调用
 * @retval 0=成功，0xFF=任务创建失败，0xFE=任务已创建
 */
uint8_t TOF_StartTask(void);

/**
 * @brief  兼容原有接口：初始化+自动启动任务
 * @retval 0=成功，非0=失败
 */
uint8_t TOF_InitAndStartTask(void);

#ifdef __cplusplus
}
#endif

#endif /* __TOF_PORT_H__ */