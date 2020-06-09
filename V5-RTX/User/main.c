#include "includes.h"

/*
**********************************************************************************************************
											��������
**********************************************************************************************************
*/
static void AppTaskCreate (void);
void AppTaskUserIF(void *argument);
void AppTaskLED(void *argument);
void AppTaskMsgPro(void *argument);
void AppTaskStart(void *argument);


/*
**********************************************************************************************************
											 ����
**********************************************************************************************************
*/
/* ������������� */
const osThreadAttr_t ThreadStart_Attr = 
{
	/* δʹ�� */
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

/* ������ */
osThreadId_t ThreadIdTaskUserIF = NULL;
osThreadId_t ThreadIdTaskStatistic = NULL;
osThreadId_t ThreadIdTaskMsgPro = NULL;
osThreadId_t ThreadIdTaskLED = NULL;
osThreadId_t ThreadIdStart = NULL;


/* ͳ������ʹ�� */
__IO uint8_t   OSStatRdy;        /* ͳ�����������־ */
__IO uint32_t  OSIdleCtr;        /* ����������� */
__IO float     OSCPUUsage;       /* CPU�ٷֱ� */
uint32_t       OSIdleCtrMax;     /* 1�������Ŀ��м��� */
uint32_t       OSIdleCtrRun;     /* 1���ڿ�������ǰ���� */

/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: ��׼c������ڡ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int main (void) 
{	
	/* HAL�⣬MPU��Cache��ʱ�ӵ�ϵͳ��ʼ�� */
	System_Init();

	/* �ں˿���ǰ�ر�HAL��ʱ���׼ */
	HAL_SuspendTick();
	
	/* �ں˳�ʼ�� */
	osKernelInitialize();                                  

	/* ������������ */
	ThreadIdStart = osThreadNew(AppTaskStart, NULL, &ThreadStart_Attr);  

	/* ���������� */
	osKernelStart();
	
	while(1);
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskUserIF
*	����˵��: ������Ϣ����		
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: osPriorityNormal (��ֵԽС���ȼ�Խ�ͣ������uCOS�෴)
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
				/* K1�����£�ֱ�ӷ����¼���־������AppTaskTCPMain������bit0 */
				case KEY_DOWN_K1:
					printf("K1������\r\n");				
					break;	

				/* K2�����£�ֱ�ӷ����¼���־������AppTaskTCPMain������bit1 */
				case KEY_DOWN_K2:
					printf("K2������\r\n");
					break;
				
				/* K3�����£�ֱ�ӷ����¼���־������AppTaskTCPMain������bit2 */
				case KEY_DOWN_K3:
					printf("K3������\r\n");
					break;

				/* �����ļ�ֵ������ */
				default:                     
					break;
			}
		}
		
		osDelay(20);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskLED
*	����˵��: LED��˸��
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: osPriorityNormal1 
*********************************************************************************************************
*/
void AppTaskLED(void *argument)
{
	const uint16_t usFrequency = 100; /* �ӳ����� */
	uint32_t tick;

	/* ��ȡ��ǰʱ�� */
	tick = osKernelGetTickCount(); 
	
    while(1)
    {
		bsp_LedToggle(2);
		/* ����ӳ� */
		tick += usFrequency;                          
		osDelayUntil(tick);
    }
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskMsgPro
*	����˵��: TCPnetӦ������
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: osPriorityNormal2  
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
*	�� �� ��: AppTaskEthCheck
*	����˵��: ������߲��״̬��
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: osPriorityNormal3  
*********************************************************************************************************
*/
void  OSStatInit (void)
{
	OSStatRdy = FALSE;
	
    osDelay(2u);            /* ʱ��ͬ�� */
	
    //__disable_irq();
    OSIdleCtr    = 0uL;         /* ����м��� */
	//__enable_irq();
	
    osDelay(100);           /* ͳ��100ms�ڣ������м��� */
	
   	//__disable_irq();
    OSIdleCtrMax = OSIdleCtr;   /* ���������м��� */
    OSStatRdy    = TRUE;
	//__enable_irq();
}

void AppTaskStatistic(void *argument)
{
 while (OSStatRdy == FALSE) 
	{
        osDelay(200);     /* �ȴ�ͳ��������� */
    }

    OSIdleCtrMax /= 100uL;
    if (OSIdleCtrMax == 0uL) 
	{
        OSCPUUsage = 0u;
    }
	
    //__disable_irq();
    OSIdleCtr = OSIdleCtrMax * 100uL;  /* ���ó�ʼCPU������ 0% */
	//__enable_irq();
	
    for (;;) 
	{
        //__disable_irq();
        OSIdleCtrRun = OSIdleCtr;    /* ���100ms�ڿ��м��� */
        OSIdleCtr    = 0uL;          /* ��λ���м��� */
       	//__enable_irq();            /* ����100ms�ڵ�CPU������ */
        OSCPUUsage   = (100uL - (float)OSIdleCtrRun / OSIdleCtrMax);
		    printf("OSCPUUsage:%02f\r\n", OSCPUUsage);
    		osDelay(100);            /* ÿ100msͳ��һ�� */
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
*	�� �� ��: AppTaskStart
*	����˵��: ����������������BSP����������
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: osPriorityNormal4  
*********************************************************************************************************
*/
void AppTaskStart(void *argument)
{
	const uint16_t usFrequency = 10; /* �ӳ����� */
	uint32_t tick;
	
	/* ��ʼ������ */
	HAL_ResumeTick();
	bsp_Init();
  ThreadIdTaskStatistic = osThreadNew(AppTaskStatistic, NULL, &ThreadStatistic_Attr);
  OSStatInit();
	/* �������� */
	AppTaskCreate();

	/* ��ȡ��ǰʱ�� */
	tick = osKernelGetTickCount(); 
	
    while(1)
    {
//		/* ��Ҫ�����Դ���ĳ��򣬶�Ӧ������̵��õ�SysTick_ISR */
		bsp_ProPer1ms();
		
		/* ����ӳ� */
		tick += usFrequency;                          
		osDelayUntil(tick);
    }
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
	  	
	ThreadIdTaskLED = osThreadNew(AppTaskLED, NULL, &ThreadLED_Attr);  
	ThreadIdTaskUserIF = osThreadNew(AppTaskUserIF, NULL, &ThreadUserIF_Attr);  
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
