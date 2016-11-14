/**
 *  @file    usrlog.h
 *  @version 1.00.00
 *  @date    2011-07-22
 *  @brief   用户日志接口，用户日志记录ITC应用程序中的运行时信息，方便用户
 *           在离线时进行应用程序的状态分析，诊断等。
 *           日志分为用户应用日志和内核模块日志，内核模块日志比应用日志的级别
 *           高。
 *           @note
 *           日志文件中内容的记录格式：\n
 *           Date       Time     O L Contents\n
 *           2011-07-22 13:38:02 U W Network is closed.\n
 *           2011-07-22 13:40:23 U M New connection in.\n
 *           ......\n
 *           - O:日志来源
 *            -# U - 用户空间。
 *            -# K - 内核空间。
 *           - L:日志级别
 *            -# M - 消息日志。
 *            -# W - 警告日志。
 *            -# E - 错误日志。
 */

#ifndef __usrlog_H
#define __usrlog_H

/**
 *  @brief 日志记录级别
 */
#define LOG_LEVEL_MSG 1 ///< 记录一般信息。
#define LOG_LEVEL_WRN 2 ///< 记录警告信息。
#define LOG_LEVEL_ERR 4 ///< 记录错误信息。
#define LOG_LEVEL_ALL 7 ///< 记录以上所有信息。


#define	LOG_IOCTL_BASE	'L'

#define	LOGIOC_SETLEVEL	_IOW(LOG_IOCTL_BASE, 0, int)
//#define	LOGIOC_SETLEVEL	_IOW(LOG_IOCTL_BASE, 0, 4)
#define	LOGIOC_CLRALL	_IO(LOG_IOCTL_BASE, 1)


/**
 *  @brief 日志记录属性
 */
typedef struct LogAttribute 
{
	char* name;	///< 日志记录文件名。
	int  level; ///< 日志记录级别。
}LogAttribute;

/* function declarations */

/**
 * @brief	创建日志对象。
 * @param	pAttr - 日志对象的属性。
 * @return	日志对象句柄 -成功，NULL-失败。
 */
extern void* Log_create( LogAttribute *pAttr );

/**
 * @brief	对日志模块释放。
 * @param	handle -日志句柄。
 * @return	0 – 成功，非0 – 错误，高16位表示模块号，低16位表示出错误号。
 */
 extern int Log_delete( void* handle );

/**
 * @brief	向日志对象写日志记录。
 * @param	handle - 日志对象句柄。
 * @param	usrlog - 1：用户日志，0：内核模块日志。
 * @param	level - 日志记录级别。
 * @param	pBuf - 数据缓冲区指针。
 * @return	成功写入的字节数量，-1 - 失败。
 */
extern int Log_write( void* handle, int usrlog, int level, char* pBuf );

/**
 * @brief	向日志对象读取一条日志记录。
 * @param	handle - 日志对象句柄。
 * @param	pBuf - 写入缓冲区指针。
 * @param	len - 缓冲区长度。
 * @return	0 - 成功，-1 - 失败，1 - 到文件末尾。
 */
extern int Log_read( void* handle, char* pBuf, int len );

#endif

// End of file
