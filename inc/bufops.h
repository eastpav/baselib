/**
 *  @file  bufops.h
 *  @version 1.00.00
 *  @date  2011-07-18
 *  @brief 缓冲区操作接口，可以使用C标准库中的memxxx(...)函数接口替代。
 */

#ifndef _BUFOPS_H
#define _BUFOPS_H

/* function declarations */

/**
 * @brief	将缓冲区中指定的字节转置，例如{1,2,3,4,5}转变为{5,4,3,2,1}。
 * @param	buf - 缓冲区指针。
 * @param	nbytes - 需要转置的字节数量。
 * @return	无。
 */
extern void binvtBytes( char *buf, int nbytes );

/**
 * @brief	将源缓冲区指定数量的字节按字节拷贝到目标缓冲区。
 * @param	source - 源缓冲区指针。
 * @param	destination - 目标缓冲区指针。
 * @param	nbytes - 拷贝的字节数量。
 * @return	无。
 */
extern void bcopyBytes( char *source, char *destination, int nbytes );

/**
 * @brief	将源缓冲区指定数量的字按字拷贝到目标缓冲区，源和目标缓冲区需要
 *          按字对齐【2字节】。
 * @param	source - 源缓冲区指针。
 * @param	destination - 目标缓冲区指针。
 * @param	nwords - 拷贝的字数量。
 * @return	无。
 */
extern void bcopyWords( char *source, char *destination, int nwords );

/**
 * @brief	将源缓冲区指定数量的双字按双字拷贝到目标缓冲区，源和目标缓冲区需要
 *          按双字对齐【4字节】。
 * @param	source - 源缓冲区指针。
 * @param	destination - 目标缓冲区指针。
 * @param	nlongs - 拷贝的双字数量。
 * @return	无。
 */
extern void bcopyLongs( char *source, char *destination, int nlongs );

/**
 * @brief	将源缓冲区指定数量的字节填充为指定的字符。
 * @param	buf - 源缓冲区指针。
 * @param	nbytes - 填充的字节长度。
 * @param	ch - 指定的填充字节。
 * @return	无。
 */
extern void bfillBytes( char *buf, int nbytes, int ch );

#endif /*_BUFOPS_H*/

/*
// End of file 
*/
