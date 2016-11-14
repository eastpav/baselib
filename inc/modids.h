/**
 *  @file  modids.h
 *  @version 1.00.00
 *  @date  2011-07-18
 *  @brief ITC模块ID和错误码定义文件，ITCDK中的大部分接口的返回值包括
 *         模块ID + 错误代码，其中错误代码包括系统定义的错误代码和ITCDK
 *         自定义的错误代码。
 */

#ifndef __modids_H
#define __modids_H

/**
 *  @brief 模块ID定义
 */
#define CAPTURE_MODID		1 ///< 视频接口模块ID
#define AFE_MODID			2 ///< AEF模块ID
#define TRIGGER_MODID		3 ///< 触发模块ID
#define GIO_MODID			4 ///< GIO模块ID
#define IMGCHNL_MODID		5 ///< 图像通道模块ID
#define SERVER_MODID		6 ///< 通信服务器模块ID
#define CONNECTION_MODID	7 ///< 通信连接模块ID
#define STORAGE_MODID		8 ///< 存储模块ID
#define CTRLCHNL_MODID		9 ///< 远程控制模块ID
#define LOG_MODID			10 ///< 日志模块ID

/**
 *  @brief 模块的错误代码，高16位为模块的ID，低16位为错误号，错误编号包括系统定义的错误号，
 *         同时还包括自定义的错误号。
 */
#define MOD_ERRCODE(id, code) ((id << 16) | (code))

/**
 *  @brief 从错误码中提取模块ID
 */
#define MOD_ID(code) ((code) >> 16)

/**
 *  @brief 从错误码中提取实际的错误号
 */
#define MOD_ERROR(code) ((code) & 0xffff)

/**
 *  @brief 自定义错误号的起始基数，其中1-4095为操作系统保留
 */
#define MOD_ERRBASE 4096

/**
 *  @brief 自定义的错误号
 */
#define EMOD_INVALID (MOD_ERRBASE + 0) ///< 非法的句柄或指针。
#define EMOD_NOIMPL  (MOD_ERRBASE + 1) ///< 指定的命令未实现。
#define EMOD_NOINIT  (MOD_ERRBASE + 2) ///< 模块没有初始化。
#define EMOD_RANGE   (MOD_ERRBASE + 3) ///< 错误的值范围。

#define EMOD_AFE_BUSY (MOD_ERRBASE + 4) ///< AFE前端队列满。
#endif

// End of file
