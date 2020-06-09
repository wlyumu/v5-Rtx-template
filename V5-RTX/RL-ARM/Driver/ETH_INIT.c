/*
*********************************************************************************************************
*
*	模块名称 : 以太网初始化
*	文件名称 : ETH_INIT.c
*	版    本 : V1.0
*	说    明 : 以太网相关变量，状态消息和以太网服务程序。
*
*	修改记录 :
*		版本号   日期         作者        说明
*		V1.0    2019-10-12   Eric2013     首版
*
*	Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"


/*
   移植注意事项：
   除了本文件中的配置，还有三个地方要根据自己制作的硬件做适配
   1、bsp.h文件中，函数MPU_Config做的以太网收发描述符地址空间的MPU配置。
   2、stm32h7xx_hal_conf.h文件做的发送ETH_TX_DESC_CNT和接收ETH_RX_DESC_CNT描述符个数配置。
   3、根据使用的以太网PHY芯片，选择合适的PHY驱动。
*/

/*
*********************************************************************************************************
*	                                  用于本文件的调试
*********************************************************************************************************
*/
#if 1
	#define printf_eth printf
#else
	#define printf_debug(...)
#endif

/*
**********************************************************************************************************
											变量
**********************************************************************************************************
*/

/* 以太网连接状态，0和1都表示初始临时状态，2表示连接上，3表示断开 */
__IO uint8_t  g_ucEthLinkStatus = 0;  

/*
*********************************************************************************************************
*	函 数 名: netETH_Notify
*	功能说明: 以太网状态消息
*	形    参: ---
*	返 回 值: 无
*********************************************************************************************************
*/
void netETH_Notify (uint32_t if_num, netETH_Event event, uint32_t val) 
{
	NET_ETH_LINK_INFO *info;

	switch (event) 
	{
		case netETH_LinkDown:
			if(g_ucEthLinkStatus == 2)
			{
				g_ucEthLinkStatus = 3;
			}
			else
			{
				g_ucEthLinkStatus = 1;
			}
			printf_eth ("Link is down\r\n");
			break;
		
		case netETH_LinkUp:
			g_ucEthLinkStatus = 2;
			printf_eth ("Link is up\r\n");
			info = (NET_ETH_LINK_INFO *)&val;
			
			switch (info->speed) 
			{
				case 0:
					printf_eth ("10 MBit\r\n");
					break;
				
				case 1:
					printf_eth ("100 MBit\r\n");
					break;
				
				case 2:
					printf_eth ("1 GBit\r\n");
					break;
			}
			
			switch (info->duplex) 
			{
				case 0:
					printf_eth ("Half duplex\r\n");
					break;
				case 1:
					printf_eth ("Full duplex\r\n");
					break;
			}
			break;
		
		case netETH_Wakeup:
			printf_eth ("Wakeup frame received\r\n");
			break;
		
		case netETH_TimerAlarm:
			printf_eth ("Timer alarm\r\n");
			break;
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
