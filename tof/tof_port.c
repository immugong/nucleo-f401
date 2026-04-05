#include "tof_port.h"
#include "i2c.h"
#include <stdio.h>
// 新增：引入Shell头文件，用于命令注册和输出
#include "shell.h"

// 传感器驱动头文件（根据宏自动切换）
#if USE_VL53L7
#include "vl53l7cx_api.h"
#else
#include "vl53l8cx_api.h"
#endif

/************************ 内部私有全局变量（全部static封装，外部不可访问） ************************/
static uint8_t status, loop, isAlive, isReady;
static volatile int IntCount;
static uint8_t p_data_ready;

// 传感器配置/结果结构体（根据宏自动切换）
#if USE_VL53L7
static VL53L7CX_Configuration Dev;
static VL53L7CX_ResultsData Results;
#else
static VL53L8CX_Configuration Dev;
static VL53L8CX_ResultsData Results;
#endif

static uint8_t resolution;
static uint16_t idx;

// TOF任务句柄和创建标记（内部私有）
static osThreadId ToFTaskHandle = NULL;
static uint8_t tofTaskCreated = 0; // 0=未创建，1=已创建

/************************ 内部私有函数声明 ************************/
static uint8_t VL53LX_Sensor_Init(void);
static void StartToFTask(void const * argument);
static void get_data_by_polling(void);

/************************ 中断回调函数（TOF专属，移至此文件） ************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == INT_C_Pin)
    {
        IntCount++;
    }
}

/************************ 对外接口实现 ************************/
uint8_t TOF_Init(void)
{
    uint8_t ret;

    // 先初始化传感器
    ret = VL53LX_Sensor_Init();
    if (ret != 0)
    {
        printf("TOF Sensor Init Failed! Error Code: %d\r\n", ret);
        return ret;
    }
    printf("TOF Sensor Init All Success!\r\n");

    // 重置任务创建标记
    tofTaskCreated = 0;
    ToFTaskHandle = NULL;

    return 0;
}

uint8_t TOF_StartTask(void)
{
    // 防止重复创建任务
    if (tofTaskCreated || ToFTaskHandle != NULL)
    {
        printf("TOF Task Already Running!\r\n");
        return 0xFE;
    }

    // 创建TOF任务
    osThreadDef(ToFTask, StartToFTask, osPriorityNormal, 0, 512); // 栈大小256，避免printf溢出
    ToFTaskHandle = osThreadCreate(osThread(ToFTask), NULL);

    if (ToFTaskHandle == NULL)
    {
        printf("TOF Task Create Failed!\r\n");
        return 0xFF;
    }

    tofTaskCreated = 1;
    printf("TOF Task Create Success! Ranging will start soon...\r\n");

    return 0;
}

uint8_t TOF_InitAndStartTask(void)
{
    uint8_t ret;

    ret = TOF_Init();
    if (ret != 0)
    {
        return ret;
    }

    ret = TOF_StartTask();
    return ret;
}

/************************ 内部传感器初始化函数（从main.c移来） ************************/
static uint8_t VL53LX_Sensor_Init(void)
{
    uint8_t init_status = 0;

    // 统一I2C地址
#if USE_VL53L7
    Dev.platform.address = VL53L7CX_DEFAULT_I2C_ADDRESS;
#else
    Dev.platform.address = VL53L8CX_DEFAULT_I2C_ADDRESS;
#endif

    // 1. 检测传感器是否在线
#if USE_VL53L7
    init_status = vl53l7cx_is_alive(&Dev, &isAlive);
#else
    VL53L8CX_Reset_Sensor(&(Dev.platform));
    init_status = vl53l8cx_is_alive(&Dev, &isAlive);
#endif

    if (!isAlive || init_status != 0)
    {
#if USE_VL53L7
        printf("VL53L7CX not detected at address (0x%x)\r\n", Dev.platform.address);
#else
        printf("VL53L8CX not detected at address (0x%x)\r\n", Dev.platform.address);
#endif
        return 0xFF;
    }

    // 2. 打印初始化提示
#if USE_VL53L7
    printf("VL53L7CX Sensor initializing, please wait few seconds\r\n");
#else
    printf("VL53L8CX Sensor initializing, please wait few seconds\r\n");
#endif

    // 3. 传感器底层初始化
#if USE_VL53L7
    init_status = vl53l7cx_init(&Dev);
#else
    init_status = vl53l8cx_init(&Dev);
#endif

    if (init_status != 0)
    {
#if USE_VL53L7
        printf("VL53L7CX ULD Loading failed, err code: %d\r\n", init_status);
#else
        printf("VL53L8CX ULD Loading failed, err code: %d\r\n", init_status);
#endif
        return init_status;
    }

    // 4. 打印API版本
#if USE_VL53L7
    printf("VL53L7CX ULD ready ! (Version : %s)\r\n", VL53L7CX_API_REVISION);
#else
    printf("VL53L8CX ULD ready ! (Version : %s)\r\n", VL53L8CX_API_REVISION);
#endif

    // 5. 配置测距参数
#if USE_VL53L7
    init_status = vl53l7cx_set_resolution(&Dev, VL53L7CX_RESOLUTION_8X8);
    if (init_status == 0)
        init_status = vl53l7cx_set_ranging_frequency_hz(&Dev, 2);
    if (init_status == 0)
        init_status = vl53l7cx_set_ranging_mode(&Dev, VL53L7CX_RANGING_MODE_CONTINUOUS);
#else
    init_status = vl53l8cx_set_resolution(&Dev, VL53L8CX_RESOLUTION_4X4);
    if (init_status == 0)
        init_status = vl53l8cx_set_ranging_frequency_hz(&Dev, 1);
    if (init_status == 0)
        init_status = vl53l8cx_set_ranging_mode(&Dev, VL53L8CX_RANGING_MODE_CONTINUOUS);
#endif

    // 6. 参数配置结果
    if (init_status != 0)
    {
#if USE_VL53L7
        printf("VL53L7CX Param Config failed, err code: %d\r\n", init_status);
#else
        printf("VL53L8CX Param Config failed, err code: %d\r\n", init_status);
#endif
    }
    else
    {
        printf("VL53LX Ranging Param Config Success!\r\n");
    }

    return init_status;
}

/************************ TOF任务实现 ************************/
static void StartToFTask(void const * argument)
{
    /* 启动测距 */
    printf("ToF Ranging starts...\r\n");

#if USE_VL53L7
    status = vl53l7cx_start_ranging(&Dev);
#else
    status = vl53l8cx_start_ranging(&Dev);
#endif

    if (status == 0)
    {
        get_data_by_polling();
    }
    else
    {
        printf("ToF start ranging failed ! Err code: %d\r\n", status);
        // 任务失败，重置标记
        tofTaskCreated = 0;
        ToFTaskHandle = NULL;
        while (1)
        {
            osDelay(1000);
        }
    }
}

/************************ 数据采集函数 ************************/
static void get_data_by_polling(void)
{
    do
    {
        /* 检查数据是否就绪 */
#if USE_VL53L7
        status = vl53l7cx_check_data_ready(&Dev, &p_data_ready);
#else
        status = vl53l8cx_check_data_ready(&Dev, &p_data_ready);
#endif

        if (p_data_ready && status == 0)
        {
            /* 获取传感器分辨率和测距数据 */
#if USE_VL53L7
            status = vl53l7cx_get_resolution(&Dev, &resolution);
            status = vl53l7cx_get_ranging_data(&Dev, &Results);
#else
            status = vl53l8cx_get_resolution(&Dev, &resolution);
            status = vl53l8cx_get_ranging_data(&Dev, &Results);
#endif

            /* 循环输出各区域测距数据 */
            for (int i = 0; i < resolution; i++)
            {
                printf("Zone : %2d, Nb targets : %2u, Ambient : %4lu Kcps/spads, ",
                       i,
                       Results.nb_target_detected[i],
                       Results.ambient_per_spad[i]);

                if (Results.nb_target_detected[i] > 0)
                {
#if USE_VL53L7
                    printf("Target status : %3u, Distance : %4d mm\r\n",
                           Results.target_status[VL53L7CX_NB_TARGET_PER_ZONE * i],
                           Results.distance_mm[VL53L7CX_NB_TARGET_PER_ZONE * i]);
#else
                    printf("Target status : %3u, Distance : %4d mm\r\n",
                           Results.target_status[VL53L8CX_NB_TARGET_PER_ZONE * i],
                           Results.distance_mm[VL53L8CX_NB_TARGET_PER_ZONE * i]);
#endif
                }
                else
                {
                    printf("Target status : 255, Distance : No target\r\n");
                }
            }
            printf("\r\n");
        }
        else
        {
            osDelay(5);
        }
    } while (1);
}

/************************ 新增：tofstart串口命令实现 ************************/
int tofstart_cmd(int argc, char *argv[])
{
    uint8_t ret;
    // 获取全局shell对象（与shell_port.c中定义的一致）
    extern Shell shell;
    
    ret = TOF_StartTask();
    
    if (ret == 0)
    {
        shellPrint(&shell, "TOF task started successfully\r\n");
    }
    else if (ret == 0xFE)
    {
        shellPrint(&shell, "TOF task is already running\r\n");
    }
    else
    {
        shellPrint(&shell, "TOF task start failed! Error code: %d\r\n", ret);
    }
    
    return 0;
}

// ✅ 修正：LetterShell 3.x 4参数格式
// 参数1：命令权限（SHELL_CMD_PERM_ALL=所有用户可执行）
// 参数2：命令名
// 参数3：命令函数
// 参数4：帮助信息
SHELL_EXPORT_CMD(0, tofstart, tofstart_cmd, "Start TOF ranging task");