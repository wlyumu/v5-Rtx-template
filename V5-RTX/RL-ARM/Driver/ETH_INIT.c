/*
*********************************************************************************************************
*
*	ģ������ : ��̫����ʼ��
*	�ļ����� : ETH_INIT.c
*	��    �� : V1.0
*	˵    �� : ��̫����ر�����״̬��Ϣ����̫���������
*
*	�޸ļ�¼ :
*		�汾��   ����         ����        ˵��
*		V1.0    2019-10-12   Eric2013     �װ�
*
*	Copyright (C), 2019-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"


/*
   ��ֲע�����
   ���˱��ļ��е����ã����������ط�Ҫ�����Լ�������Ӳ��������
   1��bsp.h�ļ��У�����MPU_Config������̫���շ���������ַ�ռ��MPU���á�
   2��stm32h7xx_hal_conf.h�ļ����ķ���ETH_TX_DESC_CNT�ͽ���ETH_RX_DESC_CNT�������������á�
   3������ʹ�õ���̫��PHYоƬ��ѡ����ʵ�PHY������
*/

/*
*********************************************************************************************************
*	                                  ���ڱ��ļ��ĵ���
*********************************************************************************************************
*/
#if 1
	#define printf_eth printf
#else
	#define printf_debug(...)
#endif

/*
**********************************************************************************************************
											����
**********************************************************************************************************
*/

/* ��̫������״̬��0��1����ʾ��ʼ��ʱ״̬��2��ʾ�����ϣ�3��ʾ�Ͽ� */
__IO uint8_t  g_ucEthLinkStatus = 0;  

/*
*********************************************************************************************************
*	�� �� ��: netETH_Notify
*	����˵��: ��̫��״̬��Ϣ
*	��    ��: ---
*	�� �� ֵ: ��
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

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
