/**
 *  @file  server.h
 *  @version 1.00.00
 *  @date  2011-07-18
 *  @brief 通信服务器接口，该接口实现对输入连接的监听，数据通信。
 */

#ifndef __SERVER_H
#define __SERVER_H

/**
 *  @brief 通信模式定义
 */
typedef enum ComMode
{
	COM_MODE_NETWORK, ///< 网络传输模式。
	COM_MODE_USB	  ///< USB传输模式。
}ComMode;

/**
 *  @brief 传输协议定义
 */
typedef enum ComProtocol
{
	COM_PROT_TCPIP4, ///< TCP/IPv4传输协议。
	COM_PROT_TCPIP6, ///< TCP/IPv6传输协议。
	COM_PROT_USB	 ///< USB传输协议。
}ComProtocol;

/**
 *  @brief 服务器属性
 */
typedef struct NetServerAttribute 
{
	short port;		///< 传输端口。
	short maxCount; ///< 最大的连接数量。
}NetServerAttribute;

/*function declaration*/

/**
 * @brief	创建通信服务器对象。
 * @param	mode -传输模式。
 * @param	prot -传输协议。
 * @param	params -初始化参数，与通信实现相关。
 * @return	通信服务器对象句柄，NULL – 错误。
 */
extern void* Server_create ( ComMode mode, ComProtocol prot, void* params );

/**
 * @brief	释放通信器对象。
 * @param	handle -通信服务器对象句柄。
 * @return	0 – 成功，非0 – 错误，高16位表示模块号，低16位表示出错误号
 */
extern int Server_delete (void* handle );

/**
 * @brief	向通信服务器对象发送指定的数据。
 * @param	handle -通信服务句柄。
 * @param	hDst –发送目标描述。
 * @param	pBuffer –发送缓冲区地址。
 * @param	len –发送缓冲区字节。
 * @param	tminms –发送定时，-1为阻塞发送。
 * @return	已成功发送的字节数量，-1 – 错误。
 */
extern int Server_send ( void* handle, void* hDst, char* pBuffer, int len, int tminms );

/**
 * @brief	从通信服务器对象接收指定的数据。
 * @param	handle -通信服务句柄。
 * @param	hSrc –接收源描述。
 * @param	pBuffer –接收缓冲区地址。
 * @param	len –接收的字节长度。
 * @param	tminms –发送定时，-1为阻塞发送。
 * @return	已成功接收到的字节数量，-1 – 错误。
 */
extern int Server_recv ( void* handle, void* hSrc, char* pBuffer, int len, int tminms );

/**
 * @brief	监听通信服务器的连接。
 * @param	handle - 通信服务句柄。
 * @param	tminms – 定时监听，-1为阻塞监听。
 * @return	-1 – 错误，0 - 超时，大于0 - 客户端连接句柄。
 */
extern int Server_probe ( void* handle, int tminms );

#endif

