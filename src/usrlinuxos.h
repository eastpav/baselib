/* linuxos.h - linux operation system adapter header */

/*
modification history
-------------------- 
1.00, 2011-2-15, youyq initial 
*/

#ifndef __LINUXOS_H
#define __LINUXOS_H

#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <rawtypes.h>
#include <sllist.h>
#include <linux/param.h>

/* osa common routines */
extern void OSA_evalAbsTime( struct timespec *abstms, UINT32 tminms );
extern int  OSA_attachSigHandler( int sigid, void(*handler)(int) );

/* osa event definition */
typedef struct LinuxEvent
{
	UINT32          flag; /* event flag */
	pthread_mutex_t lock; /* mutex object */
	pthread_cond_t  cond; /* condition variable */
}
OSEvent;

/* definition of semaphore. */
typedef struct LinuxSemaphore
{
	INT32          count; /* semaphore count */
	pthread_mutex_t lock; /* mutex object */
	pthread_cond_t  cond; /* condition variable */
} 
OSSemaphore;

/* Defenition of mutex */
typedef struct LinuxMutex
{
	pthread_mutex_t lock; /* mutex object */
}
OSMutex;

/* Defenition of message queue */
typedef struct LinuxMessageQueue
{
    SL_LIST        qReady; /* message queue head */
    SL_LIST         qFree; /* free message queue head */
    void*         msgPool; /* messages pool */
    int	          maxMsgs; /* max number of messages in queue */
    int	        maxMsgLen; /* max length of message */
	pthread_mutex_t  lock; /* mutex */
	pthread_cond_t condrd; /* condition variable for pending on reading */
	pthread_cond_t condwr; /* condition variable for pending on writing */
} 
OSMessageQueue;

/* OSA task management */
typedef struct LinuxTask
{
	pthread_t       tid; /* task identifier */
	pthread_attr_t attr; /* task attribute */
	int (*entry)(void*); /* task entry routine */
	PVOID         param; /* task parameters */
}
OSTask;

/* OSA task max & min priority current supported. */
#define TASK_PRI_MAX sched_get_priority_max(SCHED_FIFO)
#define TASK_PRI_MIN sched_get_priority_min(SCHED_FIFO)

/* OSA task default priority. */
#define TASK_PRI_DEFAULT (TASK_PRI_MIN+(TASK_PRI_MAX-TASK_PRI_MIN)/2)

/* OSA task default stack size. */
#define TASK_STACKSIZE_DEFAULT 0

#endif /*__LINUXOS_H*/

/*
// End of file 
*/
