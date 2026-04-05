#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "shell.h"
#include "stm32f4xx_hal.h"
#include "usart.h"
#include "log.h"

// 串口接收环形缓冲区
#define SHELL_UART_RX_BUF_SIZE  128
static uint8_t uartRxBuffer[SHELL_UART_RX_BUF_SIZE];
// ✅ 加上volatile修饰，禁止编译器优化
static volatile uint16_t uartRxWriteIndex = 0;
static volatile uint16_t uartRxReadIndex = 0;
// ✅ 全局静态接收变量，解决第一个字符丢失问题
static uint8_t uartRxByte;

Shell shell;
char shellBuffer[512];
static SemaphoreHandle_t shellMutex;
extern UART_HandleTypeDef huart6;

short userShellWrite(char *data, unsigned short len)
{
    HAL_UART_Transmit(&huart6, (uint8_t *)data, len, HAL_MAX_DELAY);
    return len;
}

short userShellRead(char *data, unsigned short len)
{
    if (uartRxReadIndex == uartRxWriteIndex)
    {
        // 缓冲区为空时，让出CPU 1ms，降低CPU占用
        vTaskDelay(pdMS_TO_TICKS(1));
        return 0;
    }
    // Shell逐字符解析，len固定为1，只返回1个字节
    *data = uartRxBuffer[uartRxReadIndex];
    uartRxReadIndex = (uartRxReadIndex + 1) % SHELL_UART_RX_BUF_SIZE;
    return 1;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART6)
    {
        // ✅ 先保存当前收到的字节
        uint16_t nextWriteIndex = (uartRxWriteIndex + 1) % SHELL_UART_RX_BUF_SIZE;
        if (nextWriteIndex != uartRxReadIndex)
        {
            uartRxBuffer[uartRxWriteIndex] = uartRxByte;
            uartRxWriteIndex = nextWriteIndex;
        }
        // ✅ 再开启下一次接收
        HAL_UART_Receive_IT(huart, &uartRxByte, 1);
    }
}

int userShellLock(Shell *shell)
{
    xSemaphoreTakeRecursive(shellMutex, portMAX_DELAY);
    return 0;
}

int userShellUnlock(Shell *shell)
{
    xSemaphoreGiveRecursive(shellMutex);
    return 0;
}

void userShellInit(void)
{
    shellMutex = xSemaphoreCreateRecursiveMutex();
    if (shellMutex == NULL)
    {
        logError("shell mutex create failed!");
        return;
    }
    shell.write = userShellWrite;
    shell.read = userShellRead;
#if SHELL_USING_LOCK == 1
    shell.lock = userShellLock;
    shell.unlock = userShellUnlock;
#endif
    shellInit(&shell, shellBuffer, 512);
    
    // ✅ 使用全局变量开启第一次接收
    HAL_UART_Receive_IT(&huart6, &uartRxByte, 1);
    
    if (xTaskCreate(shellTask, "shell", 512, &shell, 5, NULL) != pdPASS)
    {
        logError("shell task create failed");
    }
}