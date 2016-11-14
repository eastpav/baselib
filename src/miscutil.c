/* misc_util.c - misc subroutine library */

#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>

/* Set no-delay / non-blocking mode on a fd. */
void
set_ndelay( int fd )
{
    int flags, newflags;

    flags = fcntl( fd, F_GETFL, 0 );
    if ( flags != -1 ) {
		newflags = flags | (int) O_NDELAY;
		if ( newflags != flags )
			(void) fcntl( fd, F_SETFL, newflags );
	}
}


/* Clear no-delay / non-blocking mode on fd. */
void
clear_ndelay( int fd )
{
    int flags, newflags;

    flags = fcntl( fd, F_GETFL, 0 );
    if ( flags != -1 ) {
		newflags = flags & ~ (int) O_NDELAY;
		if ( newflags != flags )
			(void) fcntl( fd, F_SETFL, newflags );
	}
}


/* 
// check if a fd is readable in specific time.
// 
// -1 - ERROR, 0 - TIMEOUT, 1 -Writable
*/
int
write_check( int fd, int tminms )
{
	fd_set  fdSet;
	struct 	timeval tm, *ptv;
	int status = 0;

	FD_ZERO( &fdSet );
	FD_CLR ( fd, &fdSet );
	FD_SET ( fd, &fdSet );

	/* blocked wait? */
	if(tminms== -1) {
		ptv = (struct timeval*)NULL;
	}
	/* timed wait */
	else{
		tm.tv_sec  = tminms / 1000;
		tm.tv_usec = tminms % 1000 * 1000;
		ptv = &tm;
	}

	status = select(fd + 1, NULL, &fdSet, NULL, ptv);
	if (status == 0) return 0;
	if (status == -1)
	{
		if (errno == EINTR) return 0;
		
		return status;
	}

	/* 
	// check if specified file descriptor is writible
	*/
	return (FD_ISSET(fd, &fdSet)) ? 1 : 0;
}

/* 
// check if a fd is readable in specific time.
// 
// RETURNS: -1 - ERROR, 0 - TIMEOUT, 1 -Readable
*/
int
read_check( int fd, int tminms )
{
	fd_set  fdSet;
	struct 	timeval tm, *ptv;
	int status = 0;

	FD_ZERO( &fdSet );
	FD_CLR ( fd, &fdSet );
	FD_SET ( fd, &fdSet );

	if(tminms == -1) 
	{
		ptv = (struct timeval*)NULL;
	}
	else 
	{
		tm.tv_sec  = tminms / 1000;
		tm.tv_usec = tminms % 1000 * 1000;
		ptv = &tm;
	}
	
	status = select( fd + 1, &fdSet, NULL, NULL, ptv );
	if (status == 0) return 0;
	if (status == -1)
	{
		if (errno == EINTR) return 0;
		
		return status;
	}

	/* 
	// check if specified file descriptor is readable
	*/
	return (FD_ISSET(fd, &fdSet)) ? 1 : 0;
}


int
file_writeall( int fd, char* buf, int len )
{
	char *ptr = buf;
	int nwrite, nleft, total;

	nwrite = 0; nleft = len; total = 0;
	/* send till end */
	while( nleft > 0) 
	{
		nwrite = write( fd, ptr, nleft );
		if(nwrite < 0) 
		{
			if (errno == EINTR || errno == EAGAIN)
				continue;
			else
				return -1;
		}
		nleft -= nwrite; ptr += nwrite; total += nwrite;
	}

	return total;
}

int
file_write( int fd, char* buf, int len, int tminms )
{
	int status = write_check( fd, tminms );
	if (status <= 0) return status;

	return file_writeall(fd, buf, len);
}

int
file_readall( int fd, char* buf, int len )
{
	char *ptr = buf;
	int nread, nleft, total;

	nread = 0; nleft = len; total = 0;
	/* send till end */
	while( nleft > 0) 
	{
		nread = read(fd, buf, nleft);
		if ( nread == -1 ) 
		{
			if (errno == EINTR || errno == EAGAIN)
				continue;
			else return -1;
		}
		nleft -= nread; ptr += nread; total += nread;
	}

	return total;
}

/* do for each connected node */
int
file_read( int fd, char* buf, int len, int tminms )
{
	int status = read_check( fd, tminms );
	if (status <= 0) return status;

	return file_writeall(fd, buf, len);
}

