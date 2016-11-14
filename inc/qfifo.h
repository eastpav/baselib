/**
 *  @file    qfifo.h
 *  @version 1.00.00
 *  @date    2011-07-18
 *  @brief   FIFO队列接口，FIFO队列应用于简单的数据通信，读线程因为在FIFO中
 *           没有数据而等待，写线程在写入新的数据时唤醒读线程。
 *           在该通信方式下实现自然的同步。
 *
 */

#ifndef	__qfifo_H
#define	__qfifo_H

/**
 * @brief	FIFO管理器。
 */
typedef struct qfifo_t 
{
	int count;		/**< 队列中元素的数量。 */
    int pipes[2];	/**< 管道描述符。 */
} qfifo_t;

/* function declarations */

/**
 * @brief	初始化FIFO管理器。
 * @param	pfifo 将初始化的FIFO管理器。
 * @return	0 -成功，-1-失败。
 */
extern int qfifo_init( qfifo_t *pfifo );

/**
 * @brief	清除FIFO管理器。
 * @param	pfifo 将初始化的FIFO管理器。
 * @return	void。
 */
extern void qfifo_cleanup( qfifo_t *pfifo );

/**
 * @brief	创建FIFO管理器。
 * @return	FIFO管理器。
 */
extern qfifo_t *qfifo_create( void );

/**
 * @brief	销毁FIFO管理器。
 * @param	pfifo FIFO管理器。
 * @return	void。
 */
extern void qfifo_destroy( qfifo_t *pfifo );

/**
 * @brief	将指定长度的数据写入FIFO。
 * @param	pfifo FIFO管理器。
 * @param	ptr 数据缓冲区。
 * @param	len 数据字节数量。
 * @return	0 -成功，-1-失败。
 */
extern int qfifo_put( qfifo_t *pfifo, char* ptr, int len );

/**
 * @brief	从FIFO读取指定长度的数据。
 * @param	pfifo FIFO管理器。
 * @param	ptr 数据缓冲区。
 * @param	len 数据字节数量。
 * @return	0 -成功，-1-失败。
 */
extern int qfifo_get( qfifo_t *pfifo, char* ptr, int len );

/**
 * @brief	获取FIFO管理器中元素的数量。
 * @param	pfifo FIFO管理器。
 * @return	FIFO中元素的数量。
 */
extern int qfifo_count( qfifo_t *pfifo );

#endif

