/* qfifo.c - buffer fifo manager source file */

#include <unistd.h>
#include <stdlib.h>
#include "qfifo.h"
#include "usrlog.h"

/* init a fifo queue */
int qfifo_init( qfifo_t *pfifo )
{
	int err;
	
	if ( pfifo == NULL ) return -1;

	err = pipe( pfifo->pipes );
	pfifo->count = 0;

	return err;
}

/* cleanup a fifo queue */
void qfifo_cleanup( qfifo_t *pfifo )
{
	if ( pfifo == NULL ) return;

	/* close pipe now */
	(void)close( pfifo->pipes[0] );
	(void)close( pfifo->pipes[1] );
}

/* create a fifo queue */
qfifo_t* qfifo_create( void )
{
	qfifo_t* pfifo = (qfifo_t*)	malloc( sizeof(qfifo_t));
	if ( pfifo )
	{
		if ( qfifo_init(pfifo) )
		{
			free( pfifo );
			pfifo = NULL;
		}
	}
	
	return pfifo;
}

/* destroy a fifo queue */
void qfifo_destroy( qfifo_t *pfifo )
{
	qfifo_cleanup( pfifo );
	free (pfifo);
}

/* put string point by ptr of len to fifo */
int qfifo_put( qfifo_t *pfifo, char* ptr, int len )
{
	int nwrite= 0;

	nwrite = write( pfifo->pipes[1], ptr, len);
	if ( nwrite != len ) {
		Log_write(NULL, 1, LOG_LEVEL_ERR, "qfifo_put() write faild\n");
		return -1;
	}
	pfifo->count += len;

    return 0;
}

int qfifo_get( qfifo_t *pfifo, char* ptr, int len )
{
	int nread = 0;

	nread = read( pfifo->pipes[0], ptr, len );
	if( nread != len ) {
		Log_write(NULL, 1, LOG_LEVEL_ERR, "qfifo_put() read faild\n");
		return -1;
	}
	pfifo->count -= len;
	if ( pfifo->count < 0  )
		pfifo->count = 0;
		
	return 0;
}

int qfifo_count( qfifo_t *pfifo )
{
	return pfifo->count;
}


