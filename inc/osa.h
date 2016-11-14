/**
 *  @file  osa.h
 *  @version 1.00.00
 *  @date  2011-07-18
 *  @brief 操作系统适配器接口，实现对操作系统通用组件的访问。
 *         - 通用组件包括：
 *          -# 事件标记。
 *          -# 互斥体。
 *          -# 信号量。
 *          -# 消息队列。
 *          -# 线程/任务。
 */

#ifndef _OSA_H_
#define _OSA_H_

#include <rawtypes.h>

#define MEMNEW(x) malloc(x) /**< 分配存储空间 */
#define MEMDEL(x) free(x)   /**< 是否存储空间 */

/* function declarations */

/**
 * @brief 初始化操作系统适配器。
 * @param init - 用户自定义初始化回调函数。
 * @param arg1 - 初始化回调函数的参数。
 * @param cleanup - 用户自定义清除回调函数。
 * @return	0 -成功，-1-失败。
 */
extern STATUS OSA_init( int(*init)(void*), void* arg1, void(*cleanup)(void) );

/**
 * @brief 清除操作系统适配器。
 * @param code - 应用程序的退出代码。
 * @return	无。
 */
extern void   OSA_exit( int code );

/**
 * @brief 操作系统延时。
 * @param msecs - 延时的计数，单位位毫秒。
 * @return	无。
 */
extern void   OSA_delay( UINT32 msecs );

/* Event interface */

/**
 * @brief 创建操作系统事件对象。
 * @return	事件对象句柄，NULL - 失败。
 */
extern HANDLE eventCreate( void );

/**
 * @brief 删除操作系统事件对象。
 * @param handle - 事件对象句柄。
 * @return	无。
 */
extern void   eventDelete( HANDLE handle );

/**
 * @brief 等待操作系统事件对象。
 * @param handle - 事件对象句柄。
 * @param tminms - 等待时间，单位毫秒，0 - 不等待，-1 - 无限等待，>0 - 计时等待。
 * @return	0 -成功，-1-失败。
 */
extern STATUS eventWait( HANDLE handle, int tminms );

/**
 * @brief 设置操作系统事件对象，即发事件信号。
 * @param handle - 事件对象句柄。
 * @return	0 -成功，-1-失败。
 */
extern STATUS eventSet( HANDLE handle );

/**
 * @brief 清除操作系统事件对象。
 * @param handle - 事件对象句柄。
 * @return 无。
 */
extern void   eventClear( HANDLE handle );

/**
 * @brief 检测指定的事件是否已设置。
 * @param handle - 事件对象句柄。
 * @return	0 -未设置，1-已设置。
 */
extern BOOL   eventIsSet( HANDLE handle );

/* Mutex interface */

/**
 * @brief 创建操作系统互斥体对象。
 * @return	互斥体对象句柄，NULL - 失败。
 */
extern HANDLE mutexCreate( void );

/**
 * @brief 删除操作系统互斥体对象。
 * @param handle - 互斥体对象句柄。
 * @return	无。
 */
extern void   mutexDelete( HANDLE handle );

/**
 * @brief 等待操作系统互斥体对象。
 * @param handle - 互斥体对象句柄。
 * @param tminms - 等待时间，单位毫秒，0 - 不等待，-1 - 无限等待，>0 - 计时等待。
 * @return	0 -成功，-1-失败。
 */
extern STATUS mutexLock( HANDLE handle, int tminms );

/**
 * @brief 释放操作系统互斥体对象。
 * @param handle - 互斥体对象句柄。
 * @return	0 -成功，-1-失败。
 */
extern STATUS mutexUnlock( HANDLE handle );

/* Semaphore interface */

/**
 * @brief 创建操作系统信号量对象。
 * @param count - 信号量计数。
 * @return	信号量对象句柄，NULL - 失败。
 */
extern HANDLE semCreate( int count );

/**
 * @brief 删除操作系统信号量对象。
 * @param handle - 信号量对象句柄。
 * @return	无。
 */
extern void   semDelete( HANDLE handle );

/**
 * @brief 等待操作系统信号量对象，如果信号量计数大于0，则立即返回，同时将信号量
          的计数减1。
 * @param handle - 信号量对象句柄。
 * @param tminms - 等待时间，单位毫秒，0 - 不等待，-1 - 无限等待，>0 - 计时等待。
 * @return	0 -成功，-1-失败。
 */
extern STATUS semWait( HANDLE handle, int tminms );

/**
 * @brief  发送操作系统信号量对象，将信号量的计数加1，如果有其他的线程中等待该信号
           量，则唤醒等待线程。
 * @param  handle - 信号量对象句柄。
 * @return 0 -成功，-1-失败。。
 */
extern STATUS semPost( HANDLE handle );

/**
 * @brief  获取操作系统信号量对象的计数。
 * @param  handle - 信号量对象句柄。
 * @return 信号量的计数，-1 - 错误。
 */
extern int    semCount( HANDLE handle );

/* Message queue interface */

#define MSG_PRI_NORMAL	0	/**< 普通优先级 */
#define MSG_PRI_URGENT	1	/**< 紧急优先级 */

/**
 * @brief  创建消息队列。
 * @param  max_msgs - 支持的最大消息数量。
 * @param  max_msg_len - 支持的最大消息长度。
 * @return 消息队列句柄，NULL-失败。
 */
extern HANDLE mqCreate( int max_msgs, int max_msg_len );

/**
 * @brief  删除消息队列。
 * @param  handle - 消息队列句柄。
 * @return 无。
 */
extern void   mqDelete( HANDLE handle );

/**
 * @brief  向消息队列发送指定的消息。
 * @param  handle - 消息队列句柄。
 * @param  buffer - 消息缓冲区。
 * @param  nbytes - 消息的字节大小。
 * @param  tminms - 等待时间，单位毫秒，0 - 不等待，-1 - 无限等待，>0 - 计时等待。
 * @param  priority - 消息发送的紧急程度，MSG_PRI_NORMAL或MSG_PRI_URGENT。
 * @return 成功发送的字节数量，-1-失败。
 */
extern int    mqSend( HANDLE handle, char *buffer, int nbytes, int tminms, int priority );

/**
 * @brief  从消息队列接收消息。
 * @param  handle - 消息队列句柄。
 * @param  buffer - 消息缓冲区。
 * @param  maxnbytes - 消息的最大字节大小。
 * @param  tminms - 等待时间，单位毫秒，0 - 不等待，-1 - 无限等待，>0 - 计时等待。
 * @return 成功接收的字节数量，-1-失败。
 */
extern int    mqReceive( HANDLE handle, char *buffer, int maxnbytes, int tminms );

/**
 * @brief  查询指定消息队列的消息数量。
 * @param  handle - 消息队列句柄。
 * @return 消息队列中消息数量。
 */
extern int    mqCount( HANDLE handle );

/* Task interface */

/**
 * @brief  按指定的属性创建任务。
 * @param  prio - 任务的优先级，有效范围和系统的实现有关，在ITC中为0-99。
 * @param  stksz - 任务栈的大小，为0则使用操作系统的默认值。
 * @param  entry - 任务的回调函数。
 * @param  arg - 任务的回调函数参数。
 * @return 操作系统任务的句柄。
 */
extern HANDLE tskCreate( int prio, int stksz, int(*entry)(void*), void *arg );

/**
 * @brief  删除指定的任务，退出任务并释放任务所占用的资源。
 * @param  handle - 任务的句柄。
 * @return 0 -成功，-1-失败。
 */
extern STATUS tskDelete( HANDLE handle );

/**
 * @brief  按指定的属性创建任务并立即投入运行。
 * @param  prio - 任务的优先级，有效范围和系统的实现有关，在ITC中为0-99。
 * @param  stksz - 任务栈的大小，为0则使用操作系统的默认值。
 * @param  entry - 任务的回调函数。
 * @param  arg - 任务的回调函数参数。
 * @return 操作系统任务的句柄。
 */
extern HANDLE tskSpawn( int prio, int stksz, int(*entry)(void*), void *arg );

/**
 * @brief  启动由tskCreate创建的任务。
 * @param  handle - 任务的句柄。
 * @return 0 -成功，-1-失败。
 */
extern STATUS tskStart( HANDLE handle );

/**
 * @brief  重新启动由tskCreate创建的任务。
 * @param  handle - 任务的句柄。
 * @return 0 -成功，-1-失败。
 */
extern STATUS tskRestart( HANDLE handle );

/**
 * @brief  设置任务的优先级。
 * @param  handle - 任务的句柄。
 * @param  prio - 优先级，一般为0-99。
 * @return 0 -成功，-1-失败。
 */
extern STATUS tskSetPriority( HANDLE handle, int prio );

/**
 * @brief  获取指定任务的优先级。
 * @param  handle - 任务的句柄。
 * @param  prio - 优先级返回地址。
 * @return 0 -成功，-1-失败。
 */
extern STATUS tskGetPriority( HANDLE handle, int *prio );

/**
 * @brief  检测任务是否为当前任务。
 * @param  handle - 任务的句柄。
 * @return 0 -否，-1-是。
 */
extern BOOL   tskSelf( HANDLE handle );

/**
 * @brief  获取指定任务的ID号。
 * @param  handle - 任务的句柄。
 * @return 操作系统任务ID号。
 */
extern int    tskGetId( HANDLE handle );

#endif /* _OSA_H_ */

/*
// End of file 
*/

