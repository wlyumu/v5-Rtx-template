/*
*********************************************************************************************************
*
*	模块名称 : TCPnet网络协议栈测试
*	文件名称 : app_tcpnet_lib.c
*	版    本 : V1.0
*	说    明 : 测试的功能说明
*              1. 强烈推荐将网线接到路由器或者交换机上面测试，因为已经使能了DHCP，可以自动获取IP地址。
*              2. 创建了一个TCP Server，而且使能了局域网域名NetBIOS，用户只需在电脑端ping armfly
*                 就可以获得板子的IP地址，端口号1001。
*              3. 用户可以在电脑端用网络调试软件创建TCP Client连接此服务器端。
*              4. 按键K1按下，发送8字节的数据给TCP Client。
*              5. 按键K2按下，发送1024字节的数据给TCP Client。
*              6. 按键K3按下，发送5MB字节的数据给TCP Client。
*
*	修改记录 :
*		版本号   日期         作者        说明
*		V1.0    2019-10-22   Eric2013     首发
*
*	Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/	
#include "includes.h"	



/*
*********************************************************************************************************
*	                                  用于本文件的调试
*********************************************************************************************************
*/
#if 1
	#define printf_debug printf
#else
	#define printf_debug(...)
#endif


/*
*********************************************************************************************************
*	                                  宏定义
*********************************************************************************************************
*/
#define PORT_NUM       1001    /* TCP服务器监听端口号 */


/*
*********************************************************************************************************
*	                                     变量
*********************************************************************************************************
*/
int32_t tcp_sock;

/* TCPnet API的返回值 */
static const char * ReVal_Table[]= 
{
	"netOK: Operation succeeded",
	"netBusy: Process is busy",
	"netError: Unspecified error",
	"netInvalidParameter: Invalid parameter specified",
	"netWrongState: Wrong state error",
	"netDriverError: Driver error",
	"netServerError: Server error",
	"netAuthenticationFailed: User authentication failed",
	"netDnsResolverError: DNS host resolver failed",
	"netFileError: File not found or file r/w error",
	"netTimeout: Operation timeout",
};

/*
*********************************************************************************************************
*	函 数 名: tcp_cb_server
*	功能说明: TCP Socket的回调函数
*	形    参: socket  句柄
*             event   事件类型
*             addr    NET_ADDR类型变量，记录IP地址，端口号。
*             buf     ptr指向的缓冲区记录着接收到的TCP数据。
*             len     记录接收到的数据个数。
*	返 回 值: 
*********************************************************************************************************
*/
uint32_t tcp_cb_server (int32_t socket, netTCP_Event event,
                        const NET_ADDR *addr, const uint8_t *buf, uint32_t len) 
{
	switch (event) 
	{
		/*
			远程客户端连接消息
		    1、数组ptr存储远程设备的IP地址，par中存储端口号。
		    2、返回数值1允许连接，返回数值0禁止连接。
		*/
		case netTCP_EventConnect:
			if (addr->addr_type == NET_ADDR_IP4) 
			{
				printf_debug("远程客户端请求连接IP: %d.%d.%d.%d  端口号:%d\r\n", 
																addr->addr[0], 
																addr->addr[1], 
																addr->addr[2], 
																addr->addr[3], 		
																addr->port);
				return (1);
			}
			else if (addr->addr_type == NET_ADDR_IP6)  
			{
				return (1);
			}
			
			return(0);

		/* Socket远程连接已经建立 */
		case netTCP_EventEstablished:
			printf_debug("Socket is connected to remote peer\r\n");
			break;

		/* 连接断开 */
		case netTCP_EventClosed:
			printf_debug("Connection has been closed\r\n");
			break;

		/* 连接终止 */
		case netTCP_EventAborted:
			break;

		/* 发送的数据收到远程设备应答 */
		case netTCP_EventACK:
			break;

		/* 接收到TCP数据帧，ptr指向数据地址，par记录数据长度，单位字节 */
		case netTCP_EventData:
			printf_debug("Data length = %d\r\n", len);
			printf ("%.*s\r\n",len, buf);
			break;
	}
	return (0);
}

/*
*********************************************************************************************************
*	函 数 名: TCPnetTest
*	功能说明: TCPnet应用
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/    
void TCPnetTest(void)
{
	int32_t iCount;
	uint8_t *sendbuf;
	uint32_t maxlen;
	netStatus res;
	const uint16_t usMaxBlockTime = 2; /* 延迟周期 */
	uint32_t EvtFlag;
	
	tcp_sock = netTCP_GetSocket (tcp_cb_server);
	
	if (tcp_sock > 0) 
	{
		res = netTCP_Listen (tcp_sock, PORT_NUM);
		printf_debug("tcp listen res = %s\r\n", ReVal_Table[res]);
		
		/* 使能TCP_TYPE_KEEP_ALIVE，会一直保持连接 */
		netTCP_SetOption (tcp_sock, netTCP_OptionKeepAlive, 1);
	}
	
	while (1) 
	{
		
		EvtFlag = osThreadFlagsWait(0x00000007U, osFlagsWaitAny, usMaxBlockTime);
		
		/* 按键消息的处理 */
		if(netTCP_GetState(tcp_sock) == netTCP_StateESTABLISHED)
		{
			switch (EvtFlag)
			{
				/* 接收到K1键按下，给远程TCP客户端发送8字节数据 */
				case KEY1_BIT0:			  
					iCount = 8;
					do
					{
						if(netTCP_SendReady(tcp_sock) == true )
						{
							maxlen  = netTCP_GetMaxSegmentSize (tcp_sock);

							iCount -= maxlen;
							
							if(iCount < 0)
							{
								/* 这么计算没问题的 */
								maxlen = iCount + maxlen;
							}
							
							sendbuf = netTCP_GetBuffer (maxlen);
							sendbuf[0] = '1';
							sendbuf[1] = '2';
							sendbuf[2] = '3';
							sendbuf[3] = '4';
							sendbuf[4] = '5';
							sendbuf[5] = '6';
							sendbuf[6] = '7';
							sendbuf[7] = '8';
							
							/* 必须使用申请的内存空间 */
							netTCP_Send (tcp_sock, sendbuf, maxlen);
						}
						
					}while(iCount > 0);
					break;

				/* 接收到K2键按下，给远程TCP客户端发送1024字节的数据 */
				case KEY2_BIT1:	
					iCount = 1024;
					do
					{
						if(netTCP_SendReady(tcp_sock) == true )
						{
							maxlen  = netTCP_GetMaxSegmentSize (tcp_sock);

							iCount -= maxlen;
							
							if(iCount < 0)
							{
								/* 这么计算没问题的 */
								maxlen = iCount + maxlen;
							}
							
							sendbuf = netTCP_GetBuffer (maxlen);
							sendbuf[0] = '1';
							sendbuf[1] = '2';
							sendbuf[2] = '3';
							sendbuf[3] = '4';
							sendbuf[4] = '5';
							sendbuf[5] = '6';
							sendbuf[6] = '7';
							sendbuf[7] = '8';
							
							/* 必须使用申请的内存空间 */
							netTCP_Send (tcp_sock, sendbuf, maxlen);
						}
						
					}while(iCount > 0);
					break;					
					
					
				/* 接收到K3键按下，给远程TCP客户端发送5MB数据 */
				case KEY3_BIT2:			  
					iCount = 5*1024*1024;				
					do
					{
						if(netTCP_SendReady(tcp_sock) == true )
						{
							maxlen  = netTCP_GetMaxSegmentSize (tcp_sock);

							iCount -= maxlen;
							
							if(iCount < 0)
							{
								/* 这么计算没问题的 */
								maxlen = iCount + maxlen;
							}
							
							sendbuf = netTCP_GetBuffer (maxlen);
							sendbuf[0] = '1';
							sendbuf[1] = '2';
							sendbuf[2] = '3';
							sendbuf[3] = '4';
							sendbuf[4] = '5';
							sendbuf[5] = '6';
							sendbuf[6] = '7';
							sendbuf[7] = '8';
							
							/* 必须使用申请的内存空间 */
							netTCP_Send (tcp_sock, sendbuf, maxlen);
						}
						
					}while(iCount > 0);
					break;
				
				 /* 其他的键值不处理 */
				default:                     
					break;
			}
		}
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
