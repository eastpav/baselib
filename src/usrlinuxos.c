/* osa.c - OSA common subroutines */

/*
modification history
-------------------- 
1.00, 2011-2-15, youyq initial 
*/

#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <bufops.h>
#include <osa.h>
#include "usrlinuxos.h"

/********************************************************************************
// C O M M O N  R O U T I N E S
********************************************************************************/
/*
// OSA_init - Initializes OSA layer
//
// RETURNS: OK if success, or ERROR.
*/
STATUS 
OSA_init( int(*init)(void*), void* arg, void(*cleanup)(void) )
{
	struct sched_param param;
	int status = 0;
	
	/*
    // Lock all memory pages associated with this process to prevent delays
    // due to process (or thread) memory being swapped out to disk and back.
    */
	if(mlockall(MCL_CURRENT | MCL_FUTURE)) 
	{
	    return ERROR;
	}
	
	/* Set the priority of this whole process to max (requires root) */
	if(setpriority(PRIO_PROCESS, 0, -20) != 0) 
	{
		status = ERROR;
	    goto init_failed;
	}
	
	/* 
	// Set process schedule policy to FIFO, and priority to 
	// TASK_PRI_DEFAULT 
	*/
	param.sched_priority = TASK_PRI_DEFAULT;
	if(sched_setscheduler(0, SCHED_FIFO, &param) == -1)	
	{
		status = ERROR;
	    goto init_failed;
	}

    /* Call init routine if have. */
	if(init != NULL) 
	{
        status = (*init)(arg);
        if (status) 
		{
            goto init_failed;
        }
    }

	/* Register cleanup routines */
	if(cleanup != NULL)	
	{
	    if (atexit(cleanup) != 0) 
	    {
		    status = ERROR;
	    	goto init_failed;
	    }
	}

    return OK;

init_failed:
    munlockall();

	return status;
}

/*
// OSA_exit - Exits OSA layer
//
// RETURNS: N/A.
*/
void OSA_exit( int code )
{
	/* unlock all memory pages of process, and exits */
	(void)munlockall();

    /* Exit the process with specific code */
	exit( code );
}

/*
// OSA_attachSigHandler - Attach signal handler 
//
// RETURNS: OK if success, or ERROR.
*/
STATUS 
OSA_attachSigHandler( int sigid, void(*handler)(int) )
{
	struct sigaction action;

	action.sa_handler = handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags   = 0;
	
	return sigaction(sigid, &action, NULL);
}

/*
// OSA_evalAbsTime - convert time value to absolute ABSTMS
//
// RETURNS: N/A.
*/
void 
OSA_evalAbsTime( struct timespec *abstms, UINT32 tminms )
{
	clock_gettime( CLOCK_REALTIME, abstms ); 
	if( tminms ) 
	{
		abstms->tv_sec  += (tminms / 1000);
		abstms->tv_nsec += (tminms % 1000 * 1000000);
	}
}

/*
// OSA_delay - Delay time in microseconds.
//
// RETURNS: N/A.
*/
void 
OSA_delay( UINT32 msecs )
{
	struct timespec delayTime, elaspedTime;
  
	delayTime.tv_sec  = msecs / 1000;
	delayTime.tv_nsec = ( msecs % 1000 ) * 1000000;

	nanosleep(&delayTime, &elaspedTime);
}

/********************************************************************************
// L I N U X  E V E N T  R O U T I N E S
********************************************************************************/
/*
// eventCreate - create and initialize a osa flag
//
// This routine creates a osa flag. 
//
// RETURNS: Handle to OS flag, or NULL if error.
*/
HANDLE
eventCreate( void )
{
	OSEvent* pEvent = (OSEvent*)MEMNEW(sizeof(OSEvent));
	int status = OK;
	
    if (pEvent == NULL) return (NULL);
    
    status = pthread_mutex_init( &pEvent->lock, NULL );
	status |= pthread_cond_init( &pEvent->cond, NULL );
	if (status) 
	{
		MEMDEL( pEvent );
		return (NULL);
	}

	pEvent->flag = 0;

    return (HANDLE)pEvent;
}

/*
// eventDelete - delete a osa flag.
//
// This routine deletes a osa flag.
//
// RETURNS: N/A.
*/
void
eventDelete( HANDLE handle )
{
	OSEvent* pEvent = (OSEvent*)handle;
	
	if (pEvent==NULL) return;
	
	pEvent->flag   = 0;
	pthread_cond_destroy(&pEvent->cond);
	pthread_mutex_destroy(&pEvent->lock);

	MEMDEL((char*)pEvent);
}

/* 
// eventWait - wait a osa flag(s) with mode and timeout.
//
// RETURNS: OK if success, otherwize ERROR.
*/
STATUS 
eventWait( HANDLE handle, int tminms )
{
	OSEvent* pEvent = (OSEvent*)handle;
	int status = OK;

	if (pEvent == NULL) return ERROR;

	pthread_mutex_lock(&pEvent->lock);
	
	if(!pEvent->flag)
	{
		if(tminms == NO_WAIT)
		{
			status = ERROR;
		}
		else if (tminms == WAIT_FOREVER) 
		{
			status = pthread_cond_wait(&pEvent->cond, &pEvent->lock);
		}
		/* Timed wait */
		else 
		{
			struct timespec abstm;

	 		/* Get absolute time */
			clock_gettime(CLOCK_REALTIME, &abstm);
			abstm.tv_sec  += (tminms / 1000);
			abstm.tv_nsec += (tminms % 1000 * 1000000);
			status = pthread_cond_timedwait(&pEvent->cond, &pEvent->lock, &abstm);
		}
	}
	pEvent->flag = 0;
	pthread_mutex_unlock(&pEvent->lock);
	
	return (status ? ERROR : OK);
}

/* 
// eventSet - set the osa flag(s)
//
// Sets the osa flag, if any thread is waiting on the flag(s), wake up it.
//
// RETURNS: OK if success, otherwize ERROR.
*/
STATUS
eventSet( HANDLE handle )
{
	OSEvent* pEvent = (OSEvent*)handle;
	int status = OK;

	if (pEvent == NULL) return ERROR;

	pthread_mutex_lock(&pEvent->lock);
	pEvent->flag = 1;
	status = pthread_cond_signal(&pEvent->cond);
	pthread_mutex_unlock(&pEvent->lock);

	return status;
}

/* 
// eventClear - clear osa flag(s).
//
// Clears specific the osa flag(s).
//
// RETURNS: N/A.
*/
void
eventClear( HANDLE handle )
{
	OSEvent* pEvent = (OSEvent*)handle;

	if (pEvent == NULL) return;

	pthread_mutex_lock(&pEvent->lock);
	pEvent->flag = 0;
	pthread_mutex_unlock(&pEvent->lock);
}

/* 
// eventIsSet - check if the osa flag(s) is set
//
// Checks if specific the osa flag(s) is set.
//
// RETURNS: TRUE if flag is set, otherwize FALSE.
*/
BOOL 
eventIsSet( HANDLE handle )
{
	OSEvent* pEvent = (OSEvent*)handle;
	BOOL done = FALSE;

	if (pEvent == NULL) return FALSE;
	
	pthread_mutex_lock(&pEvent->lock);
	done = (pEvent->flag == 1);
	pthread_mutex_unlock(&pEvent->lock);
	
	return done;    
}

/********************************************************************************
// L I N U X  M E S S A G E  Q U E U E  R O U T I N E S
********************************************************************************/
/* message node typedefs */
typedef struct MSG_NODE
{
    SL_NODE node; /* queue node */
    int	  msgLen; /* number of bytes of data */
}
MSG_NODE;

#define MSG_NODE_DATA(pNode) (((char*)pNode) + sizeof(MSG_NODE))

/* macros */

/* The following macro determines the number of bytes needed to buffer
 * a message of the specified length.  The node size is rounded up for
 * efficiency.  The total buffer space required for a pool for
 * <maxMsgs> messages each of up to <maxMsgLen> bytes is:
 *
 *    maxMsgs * MSG_NODE_SIZE (msgLen)
 */
#define MSG_NODE_SIZE(msgLen) \
	(ROUND_UP((sizeof (MSG_NODE) + msgLen), sizeof(void*)))
	
/*
// mqInit - initialize a message queue
//
// This routine initializes a message queue data structure.  Like mqCreate()
// the resulting message queue is capable of holding up to <maxMsgs> messages,
// each of up to <maxMsgLen> bytes long.
//
// RETURNS: OK if success, otherwize ERROR.
*/
LOCAL STATUS
mqInit( HANDLE handle, int maxMsgs, int maxMsgLen )
{
    void* pool    = NULL;
	OSMessageQueue* pMsgQ = (OSMessageQueue*)handle;
    int	size      = (int) maxMsgs * MSG_NODE_SIZE(maxMsgLen);
    int nodeSize  = MSG_NODE_SIZE (maxMsgLen);
	int ix        = 0;
	int status    = OK;
	
	/* clear out msg q structure */
	bfillBytes( (char*) pMsgQ, sizeof (*pMsgQ), 0 );

	status |= pthread_mutex_init(&pMsgQ->lock,  NULL);
	status |= pthread_cond_init(&pMsgQ->condrd, NULL);
	status |= pthread_cond_init(&pMsgQ->condwr, NULL); 
	if (status) return ERROR;

	pool = (void*)MEMNEW(size);
	if(pool == NULL) return ERROR;

    /* initialize internal message queues */
	sllInit( &pMsgQ->qReady );
	sllInit( &pMsgQ->qFree );
	
	pMsgQ->msgPool = pool;

    /* put msg nodes on free list */
    for (ix = 0; ix < maxMsgs; ix++) 
	{
		sllPutAtTail( &pMsgQ->qFree, (SL_NODE*)pool );
		pool = (void*)(((char*)pool) + nodeSize);
	}

    /* initialize rest of msg q */
	pMsgQ->maxMsgs    = maxMsgs;
	pMsgQ->maxMsgLen = maxMsgLen;

	return OK;
}

/*
// mqCleanup - cleanup message queue
//
// This routine cleanups a static message queue that was initialized with
// mqInit().
//
// RETURNS: N/A.
*/
LOCAL void
mqCleanup( HANDLE handle )
{
	OSMessageQueue* pMsgQ = (OSMessageQueue*)handle;
	int nmsgs = 0;

	while (nmsgs < pMsgQ->maxMsgs) 
    {
		while (sllGet(&pMsgQ->qFree) != NULL)
			nmsgs++;
		while (sllGet(&pMsgQ->qReady) != NULL)
			nmsgs++;
	}

	if(pMsgQ->msgPool != NULL)
		MEMDEL(pMsgQ->msgPool);

	pthread_cond_destroy(&pMsgQ->condrd);
	pthread_cond_destroy(&pMsgQ->condwr);
	pthread_mutex_destroy(&pMsgQ->lock); 
}

/*
// mqCreate - create and initialize a message queue
//
// This routine creates a message queue capable of holding up to <maxMsgs>
// messages, each up to <maxMsgLen> bytes long.  The routine returns 
// a message queue ID used to identify the created message queue in all 
// subsequent calls to routines in this library. 
//
// RETURNS: Handle to message queue, or NULL if error.
*/
HANDLE
mqCreate( int maxMsgs, int maxMsgLen )
{
	HANDLE handle = (HANDLE)MEMNEW(sizeof(OSMessageQueue));
	
    if (handle == NULL) return (NULL);
    
	if (mqInit( handle, maxMsgs, maxMsgLen ) != OK) 
	{
		MEMDEL( handle );
		return (NULL);
	}

    return handle;
}

/*
// mqDelete - delete a message queue
//
// This routine deletes a message queue.
//
// RETURNS: N/A.
*/
void
mqDelete( HANDLE handle )
{
	if (handle==NULL) return;
	
	mqCleanup( handle );
	MEMDEL((char*)handle);
	handle = NULL;
}

/*
// mqSend - send a message to a message queue
//
// This routine sends the message in <buffer> of length <nBytes> to the message
// queue <pMsgQ>.  
//
// RETURNS: number of bytes received, or ERROR if failed.
*/
int 
mqSend( HANDLE handle, char *buffer, int nbytes, int tminms, int priority )
{
    MSG_NODE* p_msg;
	OSMessageQueue* pMsgQ = (OSMessageQueue*)handle;
	int status = OK;

	if((pMsgQ == NULL) || (nbytes > pMsgQ->maxMsgLen))
		return ERROR;

	pthread_mutex_lock(&pMsgQ->lock);
	while(1) 
    {
		p_msg = (MSG_NODE*)sllGet( &pMsgQ->qFree );
		if (p_msg == NULL) 
        {
			/* Non-blocking wait, set EAGAIN if message queue is empty.	*/
			if(tminms == NO_WAIT) 
            {
				status = ERROR;
			}
			/* Blocking wait */
			else if (tminms == WAIT_FOREVER) 
            {
				status = pthread_cond_wait(&pMsgQ->condwr, &pMsgQ->lock);
			}
			/* Timed wait */
			else 
            {
				struct timespec abstm;

		        clock_gettime(CLOCK_REALTIME, &abstm);
		        abstm.tv_sec  += (tminms / 1000);
		        abstm.tv_nsec += (tminms % 1000 * 1000000);
		        status = pthread_cond_timedwait(&pMsgQ->condwr, &pMsgQ->lock, &abstm);
			}
			/* Any errors occur, return with errno. */
		    if (status) break;
		}
		else 
        {
			bcopyBytes( buffer, MSG_NODE_DATA(p_msg), nbytes );
			if (priority != MSG_PRI_NORMAL)
				sllPutAtHead( &pMsgQ->qReady, (SL_NODE*)p_msg );
			else
				sllPutAtTail( &pMsgQ->qReady, (SL_NODE*)p_msg );
				
			p_msg->msgLen = nbytes;
			status = pthread_cond_signal(&pMsgQ->condrd);
			break;
		}
	}
	pthread_mutex_unlock(&pMsgQ->lock);

	return (status ? ERROR : nbytes);
}

/*
// mqReceive - receive a message from a message queue
//
// This routine receives a message from the message queue <pMsgQ>.
// The received message is copied into the specified <buffer>, which is
// <maxNBytes> in length.  If the message is longer than <maxNBytes>,
// the remainder of the message is discarded (no error indication
// is returned).
//
// RETURNS: number of bytes received, or ERROR if failed.
*/
int
mqReceive( HANDLE handle, char *buffer, int maxnbytes, int tminms )
{
    MSG_NODE* p_msg;
	OSMessageQueue* pMsgQ = (OSMessageQueue*)handle;
    int nret, status = OK;

	if (pMsgQ==NULL) return ERROR;
	
	pthread_mutex_lock(&pMsgQ->lock);
	while(1) 
    {
		p_msg = (MSG_NODE*)sllGet( &pMsgQ->qReady );
		if (p_msg == NULL) 
        {
			/* Non-blocking wait, set EAGAIN if message queue is empty.	*/
			if(tminms == NO_WAIT) 
            {
				status = ERROR;
			}
			/* Blocking wait */
			else if (tminms == WAIT_FOREVER) 
            {
				status = pthread_cond_wait(&pMsgQ->condrd, &pMsgQ->lock);
			}
			/* Timed wait */
			else 
            {
				struct timespec abstm;

		        clock_gettime(CLOCK_REALTIME, &abstm);
		        abstm.tv_sec  += (tminms / 1000);
		        abstm.tv_nsec += (tminms % 1000 * 1000000);
		        status = pthread_cond_timedwait(&pMsgQ->condrd, &pMsgQ->lock, &abstm);
			}
			/* Any errors occur, return with errno. */
		    if (status) break;
		}
		else 
        {
			nret = MIN(maxnbytes, p_msg->msgLen);
			bcopyBytes( MSG_NODE_DATA(p_msg), buffer, nret );
			sllPutAtTail( &pMsgQ->qFree, (SL_NODE*)p_msg );
			status = pthread_cond_signal(&pMsgQ->condwr);
			break;
		}
	}
	pthread_mutex_unlock(&pMsgQ->lock);

	return (status ? ERROR : nret);
}

/*
// mqCount - get the number of messages queued to a message queue
//
// RETURNS: The number of messages queued, or ERROR.
*/
int 
mqCount( HANDLE handle )
{
	OSMessageQueue* pMsgQ = (OSMessageQueue*)handle;
	int     count = 0;
	
	if (pMsgQ==NULL) return 0;

	pthread_mutex_lock(&pMsgQ->lock);
	count = sllCount( &pMsgQ->qReady );
	pthread_mutex_unlock(&pMsgQ->lock);
  
	return count;
}

/********************************************************************************
// L I N U X  M U T E X  R O U T I N E S
********************************************************************************/
/*
// Create a mutex.
//
// RETURNS: Handle to mutex.
*/
HANDLE mutexCreate( void )
{
	OSMutex *pMtx = (OSMutex*)MEMNEW(sizeof(OSMutex));
	
	if (pMtx == NULL) return (NULL);
	
	if (pthread_mutex_init(&pMtx->lock, NULL))
	{
		MEMDEL( pMtx );
		return (NULL);
	}

    return (void*)pMtx;
}

/*
// Delete the mutex.
//
// RETURNS: N/A.
*/
void mutexDelete( HANDLE handle )
{
	OSMutex *pMtx = (OSMutex*)handle;
	
	if (pMtx == NULL) return;
	
	pthread_mutex_destroy(&pMtx->lock);
	
	MEMDEL((char*)handle);
}

/*
// Lock the mutex.
//
// RETURNS: OK-success, otherwize ERROR
*/
STATUS mutexLock( HANDLE handle, int tminms )
{
	OSMutex *pMtx = (OSMutex*)handle;
	int status = 0;

	if (handle==NULL) return ERROR;
    
    /* non-blocking lock */
  	if(tminms == NO_WAIT) 
	{
  		status = pthread_mutex_trylock(&pMtx->lock);
  	}
    /* blocking lock */
	else if (tminms == WAIT_FOREVER)
	{
  		status = pthread_mutex_lock(&pMtx->lock);
  	}
  	/* timed lock */
	else 
	{
		struct timespec abstm;
		
		clock_gettime(CLOCK_REALTIME, &abstm);
		abstm.tv_sec  += (tminms / 1000);
		abstm.tv_nsec += (tminms % 1000 * 1000000);
		status = pthread_mutex_timedlock(&pMtx->lock, &abstm);
	}

	return status ? ERROR : OK;
}

/*
// Release the mutex.
//
// RETURNS: OK-success, otherwize ERROR
*/
STATUS mutexUnlock( HANDLE handle )
{
	OSMutex *pMtx = (OSMutex*)handle;

	if (handle==NULL) return ERROR;

	return pthread_mutex_unlock(&pMtx->lock) ? ERROR : OK;
}

/********************************************************************************
// L I N U X  S E M A P H O R E  R O U T I N E S
********************************************************************************/
/*
// Create a semaphore.
//
// RETURNS: Handle to semaphore.
*/
HANDLE semCreate( int count )
{
	int status = 0;
	OSSemaphore *pSem = (OSSemaphore*)MEMNEW(sizeof(OSSemaphore));
	
	if (pSem == NULL) return (NULL);
	
	status = pthread_mutex_init( &pSem->lock, NULL );
	status |= pthread_cond_init( &pSem->cond, NULL );
	if (status) 
	{
		MEMDEL( pSem );
		return (NULL);
	}

	pSem->count = count;

    return (HANDLE)pSem;
}

/*
// Delete the semaphore, and release the related rsrc.
//
// RETURNS: N/A.
*/
void semDelete( HANDLE handle )
{
	OSSemaphore *pSem = (OSSemaphore*)handle;
	
	if (pSem == NULL) return;
	
	pSem->count   = 0;
	pthread_cond_destroy(&pSem->cond);
	pthread_mutex_destroy(&pSem->lock);

	MEMDEL((char*)pSem);
}

/*
// Wait the semaphore.
//
// RETURNS: OK-success, otherwize ERROR
*/
STATUS semWait( HANDLE handle, int tminms )
{
	OSSemaphore *pSem = (OSSemaphore*)handle;
	int status = OK;
	
	if (handle==NULL) return ERROR;
	
	pthread_mutex_lock(&pSem->lock);
	
	if(pSem->count <= 0)
	{
		if(tminms == NO_WAIT)
		{
			status = ERROR;
		}
		else if (tminms == WAIT_FOREVER) 
		{
			status = pthread_cond_wait(&pSem->cond, &pSem->lock);
		}
		/* Timed wait */
		else 
		{
			struct timespec abstm;

	 		/* Get absolute time */
			clock_gettime(CLOCK_REALTIME, &abstm);
			abstm.tv_sec  += (tminms / 1000);
			abstm.tv_nsec += (tminms % 1000 * 1000000);
			status = pthread_cond_timedwait(&pSem->cond, &pSem->lock, &abstm);
		}
	}
	-- pSem->count;
	pthread_mutex_unlock(&pSem->lock);
	
	return (status ? ERROR : OK);
}

/*
// Post the semaphore.
//
// RETURNS: OK-success, otherwize ERROR
*/
STATUS semPost( HANDLE handle )
{
	int status = 0;
	OSSemaphore *pSem = (OSSemaphore*)handle;
	
	if (handle==NULL) return ERROR;
	
	pthread_mutex_lock(&pSem->lock);
	++ pSem->count;
	status = pthread_cond_signal(&pSem->cond);
	pthread_mutex_unlock(&pSem->lock);
	
	return status ? ERROR : OK;
}

/* 
// Get semaphore count.
//
// RETURNS: count of current semaphore.
*/
int semCount( HANDLE handle )
{
	OSSemaphore *pSem = (OSSemaphore*)handle;
	int count = -1;
	
	if (handle==NULL) return -1;
	pthread_mutex_lock(&pSem->lock);
	count = pSem->count;
	pthread_mutex_unlock(&pSem->lock);
	
	return count;
}

/********************************************************************************
// L I N U X  T H R E A D  R O U T I N E S
********************************************************************************/

/* Thread execute function. */
static int doExecute( OSTask* pTask )
{
    errno = 0;
	/* default cancel type is anynchronous */
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	/* realy call task function */
	return (*pTask->entry)(pTask->param);
}

/*
// Initialize the task.
//
// RETURNS: 0-success, otherwize ERROR
*/
LOCAL int tskInit( OSTask *const pTask, int prio, int stksz, int(*entry)(void*), void *arg )
{
	struct sched_param param;
	int status = 0;
	
	if(pTask==NULL) return ERROR;
	
	status = pthread_attr_init(&pTask->attr);
	if (status) return ERROR;
	
	/* set thread attributes explicitly, includes schedule policy, priority etc. */
	status |= pthread_attr_setinheritsched(&pTask->attr, PTHREAD_EXPLICIT_SCHED);
	status |= pthread_attr_setschedpolicy(&pTask->attr, SCHED_FIFO);
	status |= pthread_attr_setscope(&pTask->attr, PTHREAD_SCOPE_SYSTEM);

	/* set thread priority */
	if(prio > TASK_PRI_MAX) prio = TASK_PRI_MAX;
	if(prio < TASK_PRI_MIN) prio = TASK_PRI_MIN;
	param.sched_priority = prio;
	status |= pthread_attr_setschedparam(&pTask->attr, &param);

	/* set thread stack size */
	if(stksz != TASK_STACKSIZE_DEFAULT)	
	{
		status |= pthread_attr_setstacksize(&pTask->attr, stksz);
	}
	
	pTask->tid = 0;
	/* Store task entry and argumnets */
	pTask->entry = entry;
	pTask->param = arg;
	
	return status ? ERROR : OK;
}

/* Cleanup task memory */
LOCAL void tskCleanup( HANDLE handle )
{
	MEMDEL((char*)handle);
}

HANDLE tskCreate( int prio, int stksz, int(*entry)(void*), void *arg )
{
	HANDLE handle = (HANDLE)MEMNEW(sizeof(OSTask));
	
    if (handle == NULL) return (NULL);
	if (tskInit((OSTask*)handle, prio, stksz, entry, arg ) != OK) 
	{
		MEMDEL( handle );
		return (NULL);
	}
	
    return handle;
}

/* Exit task and release assotiate resources */
STATUS tskDelete( HANDLE handle )
{
	OSTask* pTask = (OSTask*)handle;
	int status = 0;

	if( handle==NULL ) return ERROR;
	
	pthread_attr_destroy(&pTask->attr);
	
	/* cleanup thread resource reguardless myself or other task. */
	pthread_cleanup_push(tskCleanup, (void*)handle);
	
	/* if task is running on self, exit it directly. */
	if (tskSelf( pTask ))
	{
		pthread_exit((void*)0);
	}
	else 
	{ 
		/* otherwize cancel it and wait task to end. */
		pthread_cancel(pTask->tid);
		pthread_join(pTask->tid, (void**)NULL);
	}
	pthread_cleanup_pop(1);
	
	return status ? ERROR : OK;
}

/* Create a task and execute it right now */
HANDLE tskSpawn( int prio, int stksz, int(*entry)(void*), void *arg )
{
	HANDLE handle = tskCreate( prio, stksz, entry, arg );
	
    if (handle == NULL) return (NULL);
    
	if (tskStart( handle ) != OK) 
	{
		OSTask* pTask = (OSTask*)handle;
		pthread_attr_destroy(&pTask->attr);
		MEMDEL( handle );
		return (NULL);
	}

    return handle;
}

/* 
// Start the task.
//
// RETURNS: OK if success, otherwize ERROR.
*/
STATUS tskStart( HANDLE handle )
{
	OSTask* pTask = (OSTask*)handle;
	int status = 0;

	if( pTask==NULL ) return ERROR;
	
	/* create and run thread now */
	status = pthread_create(&pTask->tid, &pTask->attr, (void*(*)(void*))doExecute, pTask);
	if(status) 
	{
		pTask->tid = 0;
		pthread_attr_destroy(&pTask->attr);
	}

	return status ? ERROR : OK;
}

/* Restart assistent thread ID. */
LOCAL pthread_t taskSelfRestartId = 0;

/* Restart assistent thread routine. */
LOCAL void* taskSelfRestartFxn( void* arg )
{
	OSTask *pTask = (OSTask*)arg;

	/* 
	// Detach our thread's pthread to clean up memory, 
	// etc. without a 'join' operation. 
    */
	pthread_detach(taskSelfRestartId);

	/* Start task now. */
	(void) tskStart(pTask);

	return (void*)0;
}

/* 
// Restarts the task.
//
// RETURNS: OK if success, otherwize ERROR.
*/
STATUS tskRestart( HANDLE handle )
{
	OSTask* pTask = (OSTask*)handle;
	int status = 0;

	if(pTask==NULL) return ERROR;
	
	/* Check if exit other thread */
	if (!pthread_equal( pTask->tid, pthread_self())) 
	{
		/* Exit target thread */
		status = pthread_cancel(pTask->tid);
		if (status) return ERROR;
		
		/* Wait cancelled task to exit */
		status = pthread_join(pTask->tid, (void **)NULL);
		if (status) return ERROR;

		pTask->tid = 0;

		/* Re-create the thread now */
		status = pthread_create(&pTask->tid, &pTask->attr, (void*(*)(void*))doExecute, pTask);
	}
	else 
	{
		/* If on myself, Create assistant thread to restart myself */
		status = pthread_create(&taskSelfRestartId, NULL, taskSelfRestartFxn, pTask);
		if (status) return ERROR;
		pthread_exit((void*)0);
	}

	return status ? ERROR : OK;
}

/* 
// Set priority of the task.
//
// set task to specified priority, range of priority is implemented
// by OS enviromnet, if handle is NULL, set the calling task priority.
//
// RETURNS: 0-success, otherwize ERROR.
*/
STATUS tskSetPriority( HANDLE handle, int prio )
{
	struct sched_param param;
	pthread_t thrid;
	OSTask* pTask = (OSTask*)handle;
	int policy, status = 0;
	
	if(pTask == NULL) 
		thrid = pthread_self();
	else 
		thrid = pTask->tid;

	if(prio > TASK_PRI_MAX) prio = TASK_PRI_MAX;
	if(prio < TASK_PRI_MIN) prio = TASK_PRI_MIN;
	
	status  = pthread_getschedparam(thrid, &policy, &param);
	param.sched_priority = prio;
	status |= pthread_setschedparam(thrid, policy, &param);

	return status ? ERROR : OK;
}

/* 
// Get priority of the task.
//
// set task to specified priority, range of priority is implemented
// by OS enviromnet, if handle is NULL, get the calling task priority.
//
// RETURNS: 0-success, otherwize ERROR
*/
int tskGetPriority( HANDLE handle, int *prio )
{
	struct sched_param param;
	pthread_t thrid;
	OSTask* pTask = (OSTask*)handle;
	int policy, status = 0;
	
	if(prio==NULL) return ERROR;

	if(pTask == NULL) 
		thrid = pthread_self();
	else 
		thrid = pTask->tid;
	
	status = pthread_getschedparam(thrid, &policy, &param);
	if(status == 0) *prio = param.sched_priority;

	return status ? ERROR : OK;
}

/* 
// Check if current thread is self.
//
// RETURNS: TRUE if on-self, or FALSE if not.
*/
BOOL tskSelf( HANDLE handle )
{
	OSTask* pTask = (OSTask*)handle;

	if(pTask==NULL) return FALSE;

	return pthread_equal(pTask->tid, pthread_self()) ? TRUE : FALSE;
}

/* 
// Get id of task.
//
// RETURNS: thread id of the task.
*/
int tskGetId( HANDLE handle )
{
	OSTask* pTask = (OSTask*)handle;

	return (int)((pTask == NULL) ? pthread_self() : pTask->tid);
}

/*
// End of file 
*/
