/**
  ******************************************************************************
  * 文件名程: bsp_debug_usart.c 
  * 作    者: kian
  * 版    本: V1.0
  * 编写日期: 2017-07-19
  * 功    能: 调试串口底层驱动程序：使用USART2
  ******************************************************************************
  * 说明：
  ******************************************************************************
  */

/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_debug_usart.h"

//#include <ctype.h>  //为了实现debug打印添加


/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
uint8_t uart2RcvOneData = 0;



// void fill_rec_buf(char data);  

#if 0

/* 回调函数 */
void handle_help(int32_t argc,void *cmd_arg);
void printf_hello(int32_t argc,void *cmd_arg);  
void handle_arg(int32_t argc,void * cmd_arg);
void get_task_state(int32_t argc,void *cmd_arg);


/*命令表*/  
const cmd_list_struct cmd_list[CMD_NUM]={  
/*命令        参数数目        处理函数            帮助信息                         */     
{"help",    0,          handle_help,    "help                       -打印帮助信息!"},  
{"hello",   0,          printf_hello,   "hello                      -打印HelloWorld!"},  
{"arg",     8,          handle_arg,     "arg<arg1> <arg2> ...       -测试用,打印输入的参数"},  
{"task",    0,          get_task_state, "task                       -获取任务信息"},  
}; 
#endif


/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;




void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    
    if(huart->Instance == USART2) 
    {
      //fill_rec_buf(uart2RcvOneData);
      HAL_UART_Receive_IT(&huart2, &uart2RcvOneData, 1);
    }
}


/**
  * 函数功能: 重定向c库函数printf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}

/**
  * 函数功能: 重定向c库函数getchar,scanf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fgetc(FILE * f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart2,&ch, 1, 0xffff);
  return ch;
}



#if 0

/*****************************************************************************/
/*******************    命令行解释器        START    *******************************/
/*****************************************************************************/
const char *g_pcHex = "0123456789abcdef";


/*
 * 为UARTprintf()提供一个底层串口发送函数,
 * 用来发送指定数目的字符，并返回最终发送的字符个数。
 */
int32_t UARTwrite(uint8_t *pcBuf, uint32_t ulLen)
{
     HAL_UART_Transmit(&huart2, pcBuf, ulLen, 0xffff);
  return ulLen;
}

/**  
  * 简介:   一个简单的printf函数,支持\%c, \%d, \%p, \%s, \%u,\%x, and \%X.  
  */    
 void UARTprintf( uint8_t *pcString, ...)    
{    
    uint32_t ulIdx;    
    uint32_t ulValue;       //保存从不定量参数堆栈中取出的数值型变量    
    uint32_t ulPos, ulCount;    
    uint32_t ulBase;        //保存进制基数,如十进制则为10,十六进制数则为16    
    uint32_t ulNeg;         //为1表示从变量为负数    
    uint8_t *pcStr;         //保存从不定量参数堆栈中取出的字符型变量    
    uint8_t pcBuf[32];      //保存数值型变量字符化后的字符    
    uint8_t cFill;          //'%08x'->不足8个字符用'0'填充,cFill='0';      
                            //'%8x '->不足8个字符用空格填充,cFill=' '    
    va_list vaArgP;    

    va_start(vaArgP, pcString);    
    while(*pcString)    
    {    
        // 首先搜寻非%核字符串结束字符    
        for(ulIdx = 0; (pcString[ulIdx] != '%') && (pcString[ulIdx] != '\0'); ulIdx++)    
        { }    
        UARTwrite(pcString, ulIdx);    

        pcString += ulIdx;    
        if(*pcString == '%')    
        {    
            pcString++;    

            ulCount = 0;    
            cFill = ' ';    
            again:    
            switch(*pcString++)    
            {    
                case '0': case '1': case '2': case '3': case '4':    
                case '5': case '6': case '7': case '8': case '9':    
                    {    
                        // 如果第一个数字为0, 则使用0做填充,则用空格填充)    
                        if((pcString[-1] == '0') && (ulCount == 0))    
                        {    
                            cFill = '0';    
                        }    
                        ulCount *= 10;    
                        ulCount += pcString[-1] - '0';    
                        goto again;    
                    }    
                case 'c':            
                    {    
                        ulValue = va_arg(vaArgP, unsigned long);    
                        UARTwrite((unsigned char *)&ulValue, 1);    
                        break;    
                    }    
                case 'd':       
                    {    
                        ulValue = va_arg(vaArgP, unsigned long);    
                        ulPos = 0;    

                        if((long)ulValue < 0)    
                        {    
                            ulValue = -(long)ulValue;    
                            ulNeg = 1;    
                        }    
                        else    
                        {    
                            ulNeg = 0;    
                        }    
                        ulBase = 10;            
                        goto convert;    
                    }    
                case 's':    
                    {    
                        pcStr = va_arg(vaArgP, unsigned char *);    

                        for(ulIdx = 0; pcStr[ulIdx] != '\0'; ulIdx++)    
                        {    
                        }    
                        UARTwrite(pcStr, ulIdx);    

                        if(ulCount > ulIdx)    
                        {    
                            ulCount -= ulIdx;    
                            while(ulCount--)    
                            {    
                                UARTwrite(" ", 1);    
                            }    
                        }    
                        break;    
                    }    
                case 'u':    
                    {    
                        ulValue = va_arg(vaArgP, unsigned long);    
                        ulPos = 0;    
                        ulBase = 10;    
                        ulNeg = 0;    
                        goto convert;    
                    }    
                case 'x': case 'X': case 'p':    
                    {    
                        ulValue = va_arg(vaArgP, unsigned long);    
                        ulPos = 0;    
                        ulBase = 16;    
                        ulNeg = 0;    
                        convert:   //将数值转换成字符    
                        for(ulIdx = 1; (((ulIdx * ulBase) <= ulValue) &&(((ulIdx * ulBase) / ulBase) == ulIdx)); ulIdx *= ulBase, ulCount--)         
                        { }    
                        if(ulNeg)    
                        {    
                            ulCount--;                          
                        }    
                        if(ulNeg && (cFill == '0'))    
                        {    
                            pcBuf[ulPos++] = '-';    
                            ulNeg = 0;    
                        }    
                        if((ulCount > 1) && (ulCount < 16))    
                        {    
                            for(ulCount--; ulCount; ulCount--)    
                            {    
                                pcBuf[ulPos++] = cFill;    
                            }    
                        }    

                        if(ulNeg)    
                        {    
                            pcBuf[ulPos++] = '-';    
                        }    

                        for(; ulIdx; ulIdx /= ulBase)    
                        {    
                            pcBuf[ulPos++] = g_pcHex[(ulValue / ulIdx) % ulBase];   
                        }    
                        UARTwrite(pcBuf, ulPos);    
                        break;    
                    }    
                case '%':    
                {    
                    UARTwrite(pcString - 1, 1);                      
                    break;    
                }    
                default:    
                {                        
                    UARTwrite("ERROR", 5);                      
                    break;    
                }    
            }    
        }    
    }    
        //可变参数处理结束    
        va_end(vaArgP);    
    }   

cmd_analyze_struct cmd_analyze;  

/*
 * 提供给串口中断服务程序，保存串口接收到的单个字符
 */  
void fill_rec_buf(char data)  
{  
    //接收数据   
    static uint32_t rec_count=0;  
     
    cmd_analyze.rec_buf[rec_count]=data;  
    
    if(0x0A==cmd_analyze.rec_buf[rec_count] && 0x0D==cmd_analyze.rec_buf[rec_count-1])  
    {  
       BaseType_t xHigherPriorityTaskWoken = pdFALSE;  
       rec_count=0;  
         
       /*收到一帧数据，向命令行解释器任务发送通知*/  
       vTaskNotifyGiveFromISR (xCmdAnalyzeHandle,&xHigherPriorityTaskWoken);  
         
       /*是否需要强制上下文切换*/  
       portYIELD_FROM_ISR(xHigherPriorityTaskWoken );  
    }  
    else  
    {  
       rec_count++;  
         
       /*防御性代码，防止数组越界*/  
       if(rec_count>=CMD_BUF_LEN)  
       {  
           rec_count=0;  
       }  
    }      
}  

/** 
* 使用SecureCRT串口收发工具,在发送的字符流中可能带有不需要的字符以及控制字符, 
* 比如退格键,左右移动键等等,在使用命令行工具解析字符流之前,需要将这些无用字符以 
* 及控制字符去除掉. 
* 支持的控制字符有: 
*   上移:1B 5B 41 
*   下移:1B 5B 42 
*   右移:1B 5B 43 
*   左移:1B 5B 44 
*   回车换行:0D 0A 
*  Backspace:08 
*  Delete:7F 
*/  
uint32_t get_true_char_stream(char *dest,const char *src)  
{  
   uint32_t dest_count=0;  
   uint32_t src_count=0;  
     
    while(src[src_count]!=0x0D && src[src_count+1]!=0x0A)  
    {  
       if(isprint(src[src_count]))  
       {  
           dest[dest_count++]=src[src_count++];  
       }  
       else  
       {  
           switch(src[src_count])  
           {  
                case    0x08:                          //退格键键值  
                {  
                    if(dest_count>0)  
                    {  
                        dest_count --;  
                    }  
                    src_count ++;  
                }break;  
                case    0x1B:  
                {  
                    if(src[src_count+1]==0x5B)  
                    {  
                        if(src[src_count+2]==0x41 || src[src_count+2]==0x42)  
                        {  
                            src_count +=3;              //上移和下移键键值  
                        }  
                        else if(src[src_count+2]==0x43)  
                        {  
                            dest_count++;               //右移键键值  
                            src_count+=3;  
                        }  
                        else if(src[src_count+2]==0x44)  
                        {  
                            if(dest_count >0)           //左移键键值  
                            {  
                                dest_count --;  
                            }  
                           src_count +=3;  
                        }  
                        else  
                        {  
                            src_count +=3;  
                        }  
                    }  
                    else  
                    {  
                        src_count ++;  
                    }  
                }break;  
                default:  
                {  
                    src_count++;  
                }break;  
           }  
       }  
    }  
    dest[dest_count++]=src[src_count++];  
    dest[dest_count++]=src[src_count++];  
    return dest_count;  
}  


/*字符串转10/16进制数*/  
static int32_t string_to_dec(uint8_t *buf,uint32_t len)  
{  
   uint32_t i=0;  
   uint32_t base=10;       //基数  
   int32_t  neg=1;         //表示正负,1=正数  
   int32_t  result=0;  
     
    if((buf[0]=='0')&&(buf[1]=='x'))  
    {  
       base=16;  
       neg=1;  
       i=2;  
    }  
    else if(buf[0]=='-')  
    {  
       base=10;  
       neg=-1;  
       i=1;  
    }  
    for(;i<len;i++)  
    {  
       if(buf[i]==0x20 || buf[i]==0x0D)    //为空格  
       {  
           break;  
       }  
         
       result *= base;  
       if(isdigit(buf[i]))                 //是否为0~9  
       {  
           result += buf[i]-'0';  
       }  
       else if(isxdigit(buf[i]))           //是否为a~f或者A~F  
       {  
            result+=tolower(buf[i])-87;  
       }  
       else  
       {  
           result += buf[i]-'0';  
       }                                          
    }  
   result *= neg;  
     
    return result ;  
}  



/** 
* 命令参数分析函数,以空格作为一个参数结束,支持输入十六进制数(如:0x15),支持输入负数(如-15) 
* @param rec_buf   命令参数缓存区 
* @param len       命令的最大可能长度 
* @return -1:       参数个数过多,其它:参数个数 
*/  
int32_t cmd_arg_analyze(char *rec_buf,unsigned int len)  
{  
   uint32_t i;  
   uint32_t blank_space_flag=0;    //空格标志  
   uint32_t arg_num=0;             //参数数目  
   uint32_t index[ARG_NUM];        //有效参数首个数字的数组索引  
     
    /*先做一遍分析,找出参数的数目,以及参数段的首个数字所在rec_buf数组中的下标*/  
    for(i=0;i<len;i++)  
    {  
       if(rec_buf[i]==0x20)        //为空格  
       {  
           blank_space_flag=1;                
           continue;  
       }  
        else if(rec_buf[i]==0x0D)   //换行  
       {  
           break;  
       }  
       else  
       {  
           if(blank_space_flag==1)  
           {  
                blank_space_flag=0;   
                if(arg_num < ARG_NUM)  
                {  
                   index[arg_num]=i;  
                    arg_num++;           
                }  
                else  
                {  
                    return -1;      //参数个数太多  
                }  
           }  
       }  
    }  
     
    for(i=0;i<arg_num;i++)  
    {  
        cmd_analyze.cmd_arg[i]=string_to_dec((unsigned char *)(rec_buf+index[i]),len-index[i]);  
    }  
    return arg_num;  
}  



/*
 *命令回调函数
 *打印字符串:Hello world!
 */  
void printf_hello(int32_t argc,void *cmd_arg)  
{  
   MY_DEBUGF(CMD_LINE_DEBUG,("Hello world!\n"));  
}  

/*
 *命令回调函数
 *打印每个参数
 */  
void handle_arg(int32_t argc,void * cmd_arg)  
{  
   uint32_t i;  
  // uint8_t  *arg=(uint8_t *)cmd_arg;  
   int32_t  *arg=(int32_t *)cmd_arg; 
     
    if(argc==0)  
    {  
       MY_DEBUGF(CMD_LINE_DEBUG,("无参数\n"));  
    }  
    else  
    {  
       for(i=0;i<argc;i++)  
       {  
           MY_DEBUGF(CMD_LINE_DEBUG,("第%d个参数:%d\n",(i+1),arg[i]));  
       }  
    }  
}  


/*
 *命令回调函数
 *获取OS任务信息
 */  
void get_task_state(int32_t argc,void *cmd_arg)  
{  
    const char task_state[]={'r','R','B','S','D'};  
    volatile UBaseType_t uxArraySize, x;  
    uint32_t ulTotalRunTime,ulStatsAsPercentage;  
   
    /* 获取任务总数目 */  
    uxArraySize = uxTaskGetNumberOfTasks();  
   if(uxArraySize>MAX_TASK_NUM)  
    {  
        MY_DEBUGF(CMD_LINE_DEBUG,("当前任务数量过多！\n"));  
    }  
   
    /*获取每个任务的状态信息 */  
    uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalRunTime );  
   
    #if (configGENERATE_RUN_TIME_STATS==1)  
    
    MY_DEBUGF(CMD_LINE_DEBUG,("========\n"));   
    MY_DEBUGF(CMD_LINE_DEBUG,("任务名          状态  ID    优先级  堆栈    CPU使用率\n"));  
    MY_DEBUGF(CMD_LINE_DEBUG,("--------------------------------------------------\n"));  
   
    /* 避免除零错误 */  
    if( ulTotalRunTime > 0 )  
    {  
        /* 将获得的每一个任务状态信息部分的转化为程序员容易识别的字符串格式 */  
        for( x = 0; x < uxArraySize; x++ )  
        {  
            char tmp[128];  
             
            /* 计算任务运行时间与总运行时间的百分比。*/  
            ulStatsAsPercentage =(uint64_t)(pxTaskStatusArray[ x ].ulRunTimeCounter)*100 / ulTotalRunTime;  
   
            if( ulStatsAsPercentage > 0UL )  
            {  
   
               sprintf(tmp,"%-16s%-6c%-6d%-8d%-8d%d%%",pxTaskStatusArray[ x].pcTaskName,task_state[pxTaskStatusArray[ x ].eCurrentState],  
                                                       pxTaskStatusArray[ x ].xTaskNumber,pxTaskStatusArray[ x].uxCurrentPriority,  
                                                       pxTaskStatusArray[ x ].usStackHighWaterMark,ulStatsAsPercentage);  
            }  
            else  
            {  
                /* 任务运行时间不足总运行时间的1%*/  
                sprintf(tmp,"%-16s%-6c%-6d%-8d%-8dt<1%%",pxTaskStatusArray[x ].pcTaskName,task_state[pxTaskStatusArray[ x ].eCurrentState],  
                                                                       pxTaskStatusArray[ x ].xTaskNumber,pxTaskStatusArray[ x].uxCurrentPriority,  
                                                                       pxTaskStatusArray[ x ].usStackHighWaterMark);                 
            }  
           MY_DEBUGF(CMD_LINE_DEBUG,("%s\n",tmp));  
        }  
    }      
    MY_DEBUGF(CMD_LINE_DEBUG,("--------------------------------------------------\n"));   
    MY_DEBUGF(CMD_LINE_DEBUG,("任务状态:   r-运行  R-就绪  B-阻塞  S-挂起  D-删除\n\n"));       
    MY_DEBUGF(CMD_LINE_DEBUG,("[堆栈]代表对应任务剩余的堆栈空间，单位是StackType_t类型，\n      这个类型在移植层定义，一般定义为4字节。\n\n"));   
    #endif //#if (configGENERATE_RUN_TIME_STATS==1)  
}  


/*
 *命令回调函数
 *获取OS任务信息
 */  
void handle_help(int32_t argc,void *cmd_arg)   
{ 
    uint8_t n = 0;
    uint8_t x = 0;

    n = ( sizeof(cmd_list) / sizeof(cmd_list[0]) );
    
    MY_DEBUGF(CMD_LINE_DEBUG,("========\n"));   
    MY_DEBUGF(CMD_LINE_DEBUG,("命令名                    说明\n"));  
    MY_DEBUGF(CMD_LINE_DEBUG,("--------------------------------------------------\n"));  
   
    /* 将获得的每一个任务状态信息部分的转化为程序员容易识别的字符串格式 */  
    for( x = 0; x < n; x++ )  
    {  
        char tmp[128];                 
          
        /* 任务运行时间不足总运行时间的1%*/  
        sprintf(tmp,"%s",cmd_list[x].help);
      
        MY_DEBUGF(CMD_LINE_DEBUG,("%s\n",tmp));  
    }  
    MY_DEBUGF(CMD_LINE_DEBUG,("\n"));
}


/*****************************************************************************/
/*******************    命令行解释器        END      *******************************/
/*****************************************************************************/

#endif

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
