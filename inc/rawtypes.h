/**
 *  @file    rawtypes.h
 *  @version 1.00.00
 *  @date    2011-07-18
 *  @brief   通用数据类型定义，包括常用的宏，数据类型的定义。
 *
 */

#ifndef __RAW_TYPES_H
#define __RAW_TYPES_H

#if	!defined(NULL)
#define NULL ((void*)0) /**< 空指针定义 */
#endif

/* Boolean type values. */
#if	!defined(FALSE) || (FALSE!=0)
#define FALSE	0  /**< 布尔值 - 假 */
#endif
#if	!defined(TRUE) || (TRUE!=1)
#define TRUE	1  /**< 布尔值 - 真 */
#endif

/* C string terminator. */
#define EOS		'\0' /**< C字符串结束符 */

/* Return status values. */
#ifndef OK
#define OK		0 /**< 返回状态 - 成功 */
#define ERROR	(-1) /**< 返回状态 - 失败 */
#endif

/* Timeout defines. */
#if	!defined(NO_WAIT) || (NO_WAIT!=0)
#define NO_WAIT		 0 /**< 超时定义 - 不等待 */
#endif
#if	!defined(WAIT_FOREVER) || (WAIT_FOREVER!=(-1))
#define WAIT_FOREVER (-1) /**< 超时定义 - 无限等待 */
#endif

/* Common macros defines. */
#define MSB(x)	(((x) >> 8) & 0xff)	  	/**< 双字节整型数的高8位 */
#define LSB(x)	((x) & 0xff)		  	/**< 双字节整型数的低8位 */
#define MSW(x) 	(((x) >> 16) & 0xffff) 	/**< 双字整型数的高8位 */
#define LSW(x) 	((x) & 0xffff) 		  	/**< 双字整型数的低8位 */

/* Swap the MSW with the LSW of a 32 bit integer. */
#define WORDSWAP(x) (MSW(x) | (LSW(x) << 16)) /**< 双字整型数的高低字交换 */

/* 32bit word byte/word swap macros. */
#define LLSB(x)		((x) & 0xff) /**< 32位整型数据的0-7位 */
#define LNLSB(x) 	(((x) >> 8 ) & 0xff) /**< 32位整型数据的8-15位 */
#define LNMSB(x) 	(((x) >> 16) & 0xff) /**< 32位整型数据的16-23位 */
#define LMSB(x)	 	(((x) >> 24) & 0xff) /**< 32位整型数据的24-32位 */
#define LONGSWAP(x) ((LLSB(x) << 24) | \
		     		(LNLSB(x) << 16) | \
		     		(LNMSB(x) << 8)  | \
		     		(LMSB(x))) /**< 32位整型数据的端模式切换，大端<->小端 */
					
/* Byte offset of member in structure */
#define OFFSETOF(structure, member) \
	((int)&(((structure*)0)->member)) /**< 结构体中成员的偏移 */

/* Size of a member of a structure */
#define MEMBER_SIZE(structure, member) \
	(sizeof(((structure*)0)->member)) /**< 结构体中成员的大小 */

/* Number of elements in an array */
#define NELEMENTS(array) \
	(sizeof(array)/sizeof((array)[0])) /**< 数组中元素的数量 */

/* Infinite loop */
#define FOREVER	for(;;) /**< 无限循环 */

#ifndef IABS
#define IABS(a)   (((a) ^ ((a) < 0 ? -1 : 0)) - ((a) < 0 ? -1 : 0)) /**< 取a绝对值 */
#endif

#ifndef MIN
#define MIN(a,b)  ((a) > (b) ? (b) : (a)) /**< 取a，b之间的最小值 */
#endif

#ifndef MAX
#define MAX(a,b)  ((a) < (b) ? (b) : (a)) /**< 取a，b之间的最大值 */
#endif

/* base types definition */
typedef char               INT8; /**< 8位整型数据 */
typedef unsigned char     UINT8; /**< 无符号8位整型数据 */
typedef short             INT16; /**< 16位整型数据 */
typedef unsigned short   UINT16; /**< 无符号16位整型数据 */
typedef int               INT32; /**< 32位整型数据 */
typedef unsigned int     UINT32; /**< 无符号32位整型数据 */
typedef long               LONG; /**< 长整型数据 */
typedef unsigned long      ULNG; /**< 无符号长整型数据 */
typedef void*             PVOID; /**< 通用指针 */
typedef PVOID            HANDLE; /**< 对象句柄 */
typedef char*             PCHAR; /**< 字符串指针 */
typedef int                BOOL; /**< 布尔类型 */
typedef int              STATUS; /**< 返回状态类型 */

/* storage class specifier definitions */
#define FAST  register /**< 寄存器存储类型 */
#define IMPORT	extern /**< 外部存储类型 */
#define LOCAL	static /**< 句柄存储类型 */

/* Round up/down macros. */
#define ROUND_UP(x, align)	 (((int)(x) + (align - 1)) & ~(align - 1)) /**< 将x按align向上对齐 */
#define ROUND_DOWN(x, align) ((int) (x) & ~(align - 1)) /**< 将x按align向下对齐 */
#define ALIGNED(x, align)	 (((int)(x) & (align - 1)) == 0) /**< 检测x是否align对齐 */

/* Rotate macros. */
#define ROR32(word, shift) ((word >> shift) | (word << (32 - shift))) /**< 将word循环右移shift位 */
#define ROL32(word, shift) ((word << shift) | (word >> (32 - shift))) /**< 将word循环左移shift位 */

/* general-purpose saturation macros */
#define CAST_8U(t)  (unsigned char)(!((t) & ~255) ? (t) : (t) > 0 ? 255 : 0) /**< 将t强制转换为8位无符号整数 */
#define CAST_8S(t)  (signed char)(!(((t)+128) & ~255) ? (t) : (t) > 0 ? 127 : -128) /**< 将t强制转换为8位有符号整数 */
#define CAST_16U(t) (unsigned short)(!((t) & ~65535) ? (t) : (t) > 0 ? 65535 : 0) /**< 将t强制转换为16位无符号整数 */
#define CAST_16S(t) (short)(!(((t)+32768) & ~65535) ? (t) : (t) > 0 ? 32767 : -32768) /**< 将t强制转换为16位整数 */

#define ZERO_OBJ(x) memset((x), 0, sizeof(*(x))) /**< 将对象x清0 */
#define NONZERO(a)  ((a) != 0) /**< 检测a是否为0 */

#endif /*__RAW_TYPES_H*/

/*
// End of file 
*/
