#ifndef __BSP_DEBUG_USART_H__
#define __BSP_DEBUG_USART_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <stdarg.h>               /*支持函数接收不定量参数*/ 
#include "string.h"


/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
#define MY_DEBUG  //打开debug 调试打印功能，若屏蔽掉则把所有的MY_DEBUGF(message)打印功能从代码中去掉
#define MAX_TASK_NUM        8  //定义最大监视任务数量，调试时候用到

#if 0
#define DEBUG_USARTx                                 USART1
#define DEBUG_USARTx_BAUDRATE                        115200
#define DEBUG_USART_RCC_CLK_ENABLE()                 __HAL_RCC_USART1_CLK_ENABLE()
#define DEBUG_USART_RCC_CLK_DISABLE()                __HAL_RCC_USART1_CLK_DISABLE()

#define DEBUG_USARTx_GPIO_ClK_ENABLE()               __HAL_RCC_GPIOA_CLK_ENABLE()
#define DEBUG_USARTx_Tx_GPIO_PIN                     GPIO_PIN_9
#define DEBUG_USARTx_Tx_GPIO                         GPIOA
#define DEBUG_USARTx_Rx_GPIO_PIN                     GPIO_PIN_10
#define DEBUG_USARTx_Rx_GPIO                         GPIOA

#define DEBUG_USART_IRQn                             USART1_IRQn
#endif
/* 扩展变量 ------------------------------------------------------------------*/
//extern UART_HandleTypeDef husart_debug;

/* 扩展函数------------------------------------------------------------------*/
//extern void UARTprintf( uint8_t *pcString, ...);    

/* 函数声明 ------------------------------------------------------------------*/
void MX_DEBUG_USART_Init(void);




/******************************************************************************/
/****************    命令行解释器实现            **************************************/
/******************************************************************************/
#define CMD_NUM         4   //命令数目，根据实际修改
#define CMD_LINE_DEBUG  1
#define MY_DBG_ON       0x01


#ifdef MY_DEBUG    
/*
*   使用方法：
*    例1： MY_DEBUGF(CMD_LINE_DEBUG, ("第%d个参数:%d\n", (i+1), arg[i]) );
*    例2：   MY_DEBUGF(CMD_LINE_DEBUG, ("无参数\n") ); 
*/
#define MY_DEBUGF(debug, message) do {\
                                if ( ((debug) & MY_DBG_ON)){UARTprintf message;}\
                               } while(0)    
#else      
#define MY_DEBUGF(message)      
#endif /* PLC_DEBUG */ 

typedef struct {  
     char const *cmd_name;                        //命令字符串  
     uint32_t max_args;                            //最大参数数目  
     void (*handle)(int argc,void * cmd_arg);     //命令回调函数  
     char  *help;                                 //帮助信息  
 }cmd_list_struct;  

#define ARG_NUM     8          //命令中允许的参数个数  
#define CMD_LEN     20         //命令名占用的最大字符长度  
#define CMD_BUF_LEN 60         //命令缓存的最大长度  
   
typedef struct {  
    char rec_buf[CMD_BUF_LEN];            //接收命令缓冲区  
    char processed_buf[CMD_BUF_LEN];      //存储加工后的命令(去除控制字符)  
    uint32_t cmd_arg[ARG_NUM];             //保存命令的参数  
}cmd_analyze_struct;  


uint32_t get_true_char_stream(char *dest,const char *src);  
int32_t cmd_arg_analyze(char *rec_buf,unsigned int len);


/******************************************************************************/
/****************    命令行解释器实现         END   ***********************************/
/******************************************************************************/



#endif  /* __BSP_DEBUG_USART_H__ */

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
