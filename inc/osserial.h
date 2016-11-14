/**
 *  @file    osserial.h
 *  @version 1.00.01
 *  @date    2011-07-25
 *  @brief   串口操作接口，实现对片上串口设备的初始化，波特率设置，串口数据
 *           读写等。
 */

#ifndef __osserial_H
#define __osserial_H

#define SERIAL_DEVICE0	"/dev/ttyS0" /**< 串口0，保留作系统调试使用。 */
#define SERIAL_DEVICE1	"/dev/ttyS1" /**< 串口1。 */
#define SERIAL_DEVICE2	"/dev/ttyS2" /**< 串口2。 */

/* function declaration */

/**
 * @brief	初始化指定串口，可以为串口1或串口2。
 * @param	name - 串口名字。
 * @param	baudrate - 串口的波特率， 从1200 到 921600，根据硬件平台的支持情况。
 * @param	dataSize - 串口的数据大小，可以是：5,6,7或8位。
 * @param	parity - 串口的奇偶特性，可以是：0 - 无, 1 - 奇校验, 2 - 偶校验。
 * @param	stopBit - 串口的停止位，可以是：0 - 1位, 1 - 2位。
 * @return	串口句柄，-1-失败。
 */
extern int  serial_init( char* name, int baudrate, int dataSize, int parity, int stopBit );

/**
 * @brief	设置串口的波特率。
 * @param	hserial - 由serial_init返回的串口句柄。
 * @param	baudrate - 串口的波特率， 从1200 到 921600，根据硬件平台的支持情况。
 * @return	0 - 成功，-1-失败。
 */
extern int  serial_setBaudrate( int hserial, int baudrate );

/**
 * @brief	初始化指定串口，可以为串口1或串口2。
 * @param	hserial - 由serial_init返回的串口句柄。
 * @param	dataSize 串口的数据大小，可以是：5,6,7或8位。
 * @return	0 - 成功，-1-失败。
 */
extern int  serial_setDataSize( int hserial, int dataSize );

/**
 * @brief	从串口中读取指定长度的数据。
 * @param	hserial - 由serial_init返回的串口句柄。
 * @param	buf - 缓冲区指针。
 * @param	len - 读取的数据长度。
 * @return	成功读取的字节数量，-1-失败。
 */
extern int  serial_read( int hserial, char* buf, int len );

/**
 * @brief	向串口中写入指定长度的数据。
 * @param	hserial - 由serial_init返回的串口句柄。
 * @param	buf - 缓冲区指针。
 * @param	len - 写入的数据长度。
 * @return	成功写入的字节数量，-1-失败。
 */
extern int  serial_write( int hserial, char* buf, int len );

/**
 * @brief	关闭指定的串口。
 * @param	hserial - 由serial_init返回的串口句柄。
 * @return	0 - 成功，-1-失败。
 */
extern int  serial_close( int hserial );

#endif

