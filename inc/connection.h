/**
 *  @file  connection.h
 *  @version 1.00.00
 *  @date  2011-07-18
 *  @brief 通信连接实现对远程通信服务器的连接，连接一般使用异步连接方式，通信的
 *         的传输模式和传输协议参见@ref server.h
 */

#ifndef __CONNECTION_H
#define __CONNECTION_H

#include <server.h>

/**
 *  @brief 通信连接服务器对象属性
 */
typedef struct NetConnPoint 
{
	int  ifaddr; ///< 通信连接服务器对象的IP地址。
	short  port; ///< 通信连接服务器对象传输端口。
	char   isOn; ///< 通信连接服务器对象连接状态，1-连接，0-未连接。
	char forced; ///< 通信连接服务器对象强制发送，1-强制发送，0-般发送。
}NetConnPoint;

/*function declaration*/

/**
 * @brief	创建通信连接对象。
 * @param	mode - 传输模式。
 * @param	prot -传输协议。
 * @param	params - 初始化参数，与通信协议相关。
 * @return	通信连接对象句柄，NULL – 错误。
 */
extern void* Connection_create (ComMode mode, ComProtocol prot, void* params );

/**
 * @brief	释放通信连接对象。
 * @param	handle -通信连接对象句柄。
 * @return	0 – 成功，非0 – 错误，高16位表示模块号，低16位表示出错误号。
 */
extern int Connection_delete ( void* handle );

/**
 * @brief	向通信连接对象发送指定长度的数据。
 * @param	handle -传输连接对象句柄。
 * @param	pBuffer –发送缓冲区地址。
 * @param	len – 发送缓冲区字节。
 * @param	tminms – 发送定时，-1为阻塞发送。
 * @return	已成功发送的字节数量，-1 – 错误。
 */
extern int Connection_send ( void* handle, char* pBuffer, int len, int tminms );

/**
 * @brief	从通信连接对象接收指定长度的数据。
 * @param	handle - 传输连接句柄。
 * @param	pBuffer – 接收缓冲区地址。
 * @param	len – 接收的字节长度。
 * @param	tminms – 发送定时，-1为阻塞发送。
 * @return	已成功接收到的字节数量，-1 – 错误。
 */
extern int Connection_recv ( void* handle, char* pBuffer, int len, int tminms );

/**
 * @brief	连接指定的远程服务器。
 * @param	handle - 通信连接对象句柄。
 * @param	pConn - 通信连接的连接点指针。
 * @param	tminms - 定时监听，-1为阻塞监听。
 * @return	1 – 成功，0 - 超时，-1 – 错误。
 */
extern int Connection_doWork (void* handle, NetConnPoint* pConn, int tminms );

#endif

