/**
 *  @file    miscutil.h
 *  @version 1.00.00
 *  @date    2011-07-18
 *  @brief   混合功能接口，提供比较常用的函数接口，如文件的读写等。
 */

#ifndef __miscutil_H
#define __miscutil_H

/* function declarations */

/**
 * @brief	设置指定的描述符为非阻塞模式。
 * @param	fd 文件描述符，可以是网络套接字。
 * @return	void。
 */
extern void set_ndelay( int fd );

/**
 * @brief	清除指定的描述符为非阻塞模式。
 * @param	fd 文件描述符，可以是网络套接字。
 * @return	void。
 */
extern void clear_ndelay( int fd );

/**
 * @brief	检查指定的描述符是否在指定的时间内可读。
 * @param	fd 文件描述符，可以是网络套接字。
 * @param	tminms 等待时间，0-立即返回，-1-无限等待，大于0-计时等待【毫秒】。
 * @return	0 -超时，1 -可写，-1-错误。
 */
extern int write_check( int fd, int tminms );

/**
 * @brief	检查指定的描述符是否在指定的时间内可写。
 * @param	fd 文件描述符，可以是网络套接字。
 * @param	tminms 等待时间，0-立即返回，-1-无限等待，大于0-计时等待【毫秒】。
 * @return	0 -超时，1 -可读，-1-错误。
 */
extern int read_check( int fd, int tminms );

/**
 * @brief	向指定的文件描述符发送数据。
 * @param	fd 文件描述符。
 * @param	buf 发送缓冲区指针。
 * @param	len 需要发送的字节数。
 * @param	tminms 等待时间，0-立即返回，-1-无限等待，大于0-计时等待【毫秒】。
 * @return	已成功发送的字节数，-1-错误。
 */
extern int file_write( int fd, char* buf, int len, int tminms );

/**
 * @brief	以阻塞的方式向指定的文件描述符发送数据。
 * @param	fd 文件描述符。
 * @param	buf 发送缓冲区指针。
 * @param	len 需要发送的字节数。
 * @return	已成功发送的字节数，-1-错误。
 */
extern int file_writeall( int fd, char* buf, int len );

/**
 * @brief	向指定的文件描述符读取数据。
 * @param	fd 文件描述符。
 * @param	buf 读取缓冲区指针。
 * @param	len 需要读取的字节数。
 * @param	tminms 等待时间，0-立即返回，-1-无限等待，大于0-计时等待【毫秒】。
 * @return	已成功读取的字节数，-1-错误。
 */
extern int file_read( int fd, char* buf, int len, int tminms );

/**
 * @brief	以阻塞的方式向指定的文件描述符读取数据。
 * @param	fd 文件描述符。
 * @param	buf 接收缓冲区指针。
 * @param	len 需要接收的字节数。
 * @return	已成功接收的字节数，-1-错误。
 */
extern int file_readall( int fd, char* buf, int len );

#endif

