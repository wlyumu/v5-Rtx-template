#include "includes.h"

/*
**********************************************************************************************************
											函数声明
**********************************************************************************************************
*/
static void AppTaskCreate (void);
void AppTaskUserIF(void *argument);
void AppTaskLED(void *argument);
void AppTaskMsgPro(void *argument);
void AppTaskStart(void *argument);


/*
**********************************************************************************************************
											 变量
**********************************************************************************************************
*/
/* 任务的属性设置 */
const osThreadAttr_t ThreadStart_Attr = 
{
	/* 未使用 */
//	.cb_mem = &worker_thread_tcb_1,
//	.cb_size = sizeof(worker_thread_tcb_1),
//	.stack_mem = &worker_thread_stk_1[0],
//	.stack_size = sizeof(worker_thread_stk_1),
//	.priority = osPriorityAboveNormal,
//	.tz_module = 0
	
	.name = "osRtxStartThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityNormal4,
	.stack_size = 2048,
};

const osThreadAttr_t ThreadStatistic_Attr = 
{
	.name = "osRtxStatisticThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityNormal,
	.stack_size = 1024,
};

const osThreadAttr_t ThreadMsgPro_Attr = 
{
	.name = "osRtxMsgProThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityNormal2,
	.stack_size = 2048,
};

const osThreadAttr_t ThreadLED_Attr = 
{
	.name = "osRtxLEDThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityNormal1,
	.stack_size = 512,
};

const osThreadAttr_t ThreadUserIF_Attr = 
{
	.name = "osRtxThreadUserIF",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityNormal,
	.stack_size = 1024,
};

/* 任务句柄 */
osThreadId_t ThreadIdTaskUserIF = NULL;
osThreadId_t ThreadIdTaskStatistic = NULL;
osThreadId_t ThreadIdTaskMsgPro = NULL;
osThreadId_t ThreadIdTaskLED = NULL;
osThreadId_t ThreadIdStart = NULL;


/* 统计任务使用 */
__IO uint8_t   OSStatRdy;        /* 统计任务就绪标志 */
__IO uint32_t  OSIdleCtr;        /* 空闲任务计数 */
__IO float     OSCPUUsage;       /* CPU百分比 */
uint32_t       OSIdleCtrMax;     /* 1秒内最大的空闲计数 */
uint32_t       OSIdleCtrRun;     /* 1秒内空闲任务当前计数 */

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: 标准c程序入口。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int main (void) 
{	
	/* HAL库，MPU，Cache，时钟等系统初始化 */
	System_Init();

	/* 内核开启前关闭HAL的时间基准 */
	HAL_SuspendTick();
	
	/* 内核初始化 */
	osKernelInitialize();                                  

	/* 创建启动任务 */
	ThreadIdStart = osThreadNew(AppTaskStart, NULL, &ThreadStart_Attr);  

	/* 开启多任务 */
	osKernelStart();
	
	while(1);
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskUserIF
*	功能说明: 按键消息处理		
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal (数值越小优先级越低，这个跟uCOS相反)
*********************************************************************************************************
*/
void AppTaskUserIF(void *argument)
{
	uint8_t ucKeyCode;

    while(1)
    {
		ucKeyCode = bsp_GetKey();
		
		if (ucKeyCode != KEY_NONE)
		{
			switch (ucKeyCode)
			{
				/* K1键按下，直接发送事件标志给任务AppTaskTCPMain，设置bit0 */
				case KEY_DOWN_K1:
					printf("K1键按下\r\n");				
					break;	

				/* K2键按下，直接发送事件标志给任务AppTaskTCPMain，设置bit1 */
				case KEY_DOWN_K2:
					printf("K2键按下\r\n");
					break;
				
				/* K3键按下，直接发送事件标志给任务AppTaskTCPMain，设置bit2 */
				case KEY_DOWN_K3:
					printf("K3键按下\r\n");
					break;

				/* 其他的键值不处理 */
				default:                     
					break;
			}
		}
		
		osDelay(20);
	}
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskLED
*	功能说明: LED闪烁。
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal1 
*********************************************************************************************************
*/
void AppTaskLED(void *argument)
{
	const uint16_t usFrequency = 100; /* 延迟周期 */
	uint32_t tick;

	/* 获取当前时间 */
	tick = osKernelGetTickCount(); 
	
    while(1)
    {
		bsp_LedToggle(2);
		/* 相对延迟 */
		tick += usFrequency;                          
		osDelayUntil(tick);
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskMsgPro
*	功能说明: TCPnet应用任务
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal2  
*********************************************************************************************************
*/
void AppTaskMsgPro(void *argument)
{
	while(1)
	{
		bsp_LedToggle(3);
		osDelay(5);
	}	
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskEthCheck
*	功能说明: 检查网线插拔状态。
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal3  
*********************************************************************************************************
*/
void  OSStatInit (void)
{
	OSStatRdy = FALSE;
	
    osDelay(2u);            /* 时钟同步 */
	
    //__disable_irq();
    OSIdleCtr    = 0uL;         /* 清空闲计数 */
	//__enable_irq();
	
    osDelay(100);           /* 统计100ms内，最大空闲计数 */
	
   	//__disable_irq();
    OSIdleCtrMax = OSIdleCtr;   /* 保存最大空闲计数 */
    OSStatRdy    = TRUE;
	//__enable_irq();
}

void AppTaskStatistic(void *argument)
{
 while (OSStatRdy == FALSE) 
	{
        osDelay(200);     /* 等待统计任务就绪 */
    }

    OSIdleCtrMax /= 100uL;
    if (OSIdleCtrMax == 0uL) 
	{
        OSCPUUsage = 0u;
    }
	
    //__disable_irq();
    OSIdleCtr = OSIdleCtrMax * 100uL;  /* 设置初始CPU利用率 0% */
	//__enable_irq();
	
    for (;;) 
	{
        //__disable_irq();
        OSIdleCtrRun = OSIdleCtr;    /* 获得100ms内空闲计数 */
        OSIdleCtr    = 0uL;          /* 复位空闲计数 */
       	//__enable_irq();            /* 计算100ms内的CPU利用率 */
        OSCPUUsage   = (100uL - (float)OSIdleCtrRun / OSIdleCtrMax);
		    printf("OSCPUUsage:%02f\r\n", OSCPUUsage);
    		osDelay(100);            /* 每100ms统计一次 */
    }
}

void osRtxIdleThread (void *argument)
{
   for(;;){
		
	  __set_PRIMASK(1);
    OSIdleCtr++;
	  __set_PRIMASK(0);

	 }

}

/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	功能说明: 启动任务，这里用作BSP驱动包处理。
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal4  
*********************************************************************************************************
*/
void AppTaskStart(void *argument)
{
	const uint16_t usFrequency = 10; /* 延迟周期 */
	uint32_t tick;
	
	/* 初始化外设 */
	HAL_ResumeTick();
	bsp_Init();
  ThreadIdTaskStatistic = osThreadNew(AppTaskStatistic, NULL, &ThreadStatistic_Attr);
  OSStatInit();
	/* 创建任务 */
	AppTaskCreate();

	/* 获取当前时间 */
	tick = osKernelGetTickCount(); 
	
    while(1)
    {
//		/* 需要周期性处理的程序，对应裸机工程调用的SysTick_ISR */
		bsp_ProPer1ms();
		
		/* 相对延迟 */
		tick += usFrequency;                          
		osDelayUntil(tick);
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
	  	
	ThreadIdTaskLED = osThreadNew(AppTaskLED, NULL, &ThreadLED_Attr);  
	ThreadIdTaskUserIF = osThreadNew(AppTaskUserIF, NULL, &ThreadUserIF_Attr);  
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
