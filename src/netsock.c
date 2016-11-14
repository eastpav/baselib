#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>
#include <sys/select.h>
#include "netsock.h"
#include "miscutil.h"
#include "usrlog.h"

#define MS_API_ERROR   -1
#define MS_API_TIMEOUT 0

/*******************************************************/
/* 非阻塞超时发送                                                         */
/* 函数:CySend_timeout               */
/* 参数: s - 必需是非阻塞的套字节                      */
/*      b - 要发送的缓存区指针                     */
/*      l - 要发送的数据长度                         */
/*      toutms - 发送超时(毫秒)                 */
/* 返回: 如果发生错误返回负数,如果发送成功返回>=0,当为0时表示  */ 
/*       数据不能立即发送,需要再次发送                   */
/*       MS_API_ERROR  超时                      */
/*       MS_API_TIMEOUT  错误                 */
/******************************************************/ 
int CySend_timeout(int s, char *b, int l, int toutms)
{
  char msg[32];
  fd_set nFdSet;
  struct timeval timeout;
  int re;
   
  FD_ZERO( &nFdSet );
  FD_SET( s, &nFdSet );
  timeout.tv_sec = toutms / 1000;
  timeout.tv_usec = (toutms % 1000) * 1000;    
  re = select(s+1, NULL, &nFdSet, NULL, &timeout);
 // printf("sel_re%d\n",re);
  switch ( re ) {
    case 0: /* time-out */
     // printf("time-out\n");
	  sprintf(msg,"CySend_timeout() faild 0 : %d\n",toutms);
	  Log_write(NULL, 1, LOG_LEVEL_WRN, msg);
      return MS_API_TIMEOUT;
      break;
    case -1: /* error */
    // printf("ERR:%d\n",errno);
	  sprintf(msg,"CySend_timeout() faild 1 : %d\n",errno);
	  Log_write(NULL, 1, LOG_LEVEL_ERR, msg);
      return MS_API_ERROR;
      break;
    default: /* Success */
      if ( FD_ISSET(s,&nFdSet) ) { /* send */
        /***************** 开始发送数据 **************/
        re = send(s, b, l, 0);
        //	printf("\nre:%d\n",re);
        if ( re == -1 ) { /* error */
          if ( errno == EAGAIN || errno == EWOULDBLOCK ) {
            return 0;
          } else {
	        sprintf(msg,"CySend_timeout() faild 2 : %d\n",errno);
	        Log_write(NULL, 1, LOG_LEVEL_ERR, msg);
            return MS_API_ERROR;
          }
        } 
        else { /* send ok */
          return re;
        }
        /***************** 完成发送数据 **************/
      }
      else {
        //printf("?-%d\n",re);
		sprintf(msg,"CySend_timeout() faild 3 : FD_ISSET\n");
	    Log_write(NULL, 1, LOG_LEVEL_ERR, msg);
        return MS_API_ERROR;
      }
  }
}

/*******************************************************/
/* 非阻塞超时接收                                                         */
/* 函数:CyRecv_timeout               */
/* 参数: s - 必需是非阻塞的套字节                       */
/*      b - 要接收的缓存区指针                      */
/*      l - 要接收的数据长度                          */
/*      toutms - 发送超时(毫秒)                   */
/* 返回: 如果发生错误返回负数,如果发送成功返回>=0,当为0时表示  */ 
/*       数据不能立即接收,需要再次接收                    */
/*       MS_API_ERROR  超时                       */
/*       MS_API_TIMEOUT  错误                  */
/******************************************************/ 
int CyRecv_timeout(int s, char *b, int l, int toutms)
{
  char msg[32];
  fd_set nFdSet;
  struct timeval timeout;
  int re;

  FD_ZERO( &nFdSet );
  FD_SET( s, &nFdSet );
  timeout.tv_sec = toutms / 1000;
  timeout.tv_usec = (toutms % 1000) * 1000;        
  re = select(s+1, &nFdSet, NULL, NULL, &timeout);
  switch ( re ) {
    case 0: /* time-out */
      //printf("time-out\n");
	  sprintf(msg,"CyRecv_timeout() faild 0 : %d\n",toutms);
	  Log_write(NULL, 1, LOG_LEVEL_WRN, msg);
      return MS_API_TIMEOUT;
      break;
    case -1: /* error */
      //printf("ERR:%d\n",errno);
	  sprintf(msg,"CyRecv_timeout() faild 1 : %d\n",errno);
	  Log_write(NULL, 1, LOG_LEVEL_ERR, msg);
      return MS_API_ERROR;
      break;
    default: /* Success */
      if ( FD_ISSET(s,&nFdSet) ) { /* recv */
        /***************** 开始接收数据 **************/
        re = recv(s, b, l, 0);
        if ( re == 0 ) { /* shutdown */
		  sprintf(msg,"CyRecv_timeout() faild 2 : %d\n",errno);
	      Log_write(NULL, 1, LOG_LEVEL_WRN, msg);
          return MS_API_ERROR;
        } else if ( re == -1 ) { /* error */
          if ( errno == EAGAIN ) {
            return 0;
          } else {
		    sprintf(msg,"CyRecv_timeout() faild 3 : %d\n",errno);
	        Log_write(NULL, 1, LOG_LEVEL_ERR, msg);
            return MS_API_ERROR;
          }
        } 
        else { /* send ok */
          return re;
        }
        /***************** 完成接收数据 **************/
      }
      else {
        //printf("?-%d\n",re);
		sprintf(msg,"CyRecv_timeout() faild 4 : FD_ISSET\n");
	    Log_write(NULL, 1, LOG_LEVEL_ERR, msg);
        return -2;
      }
  }
}


/*******************************************************/
/* 非阻塞超时接收                                                         */
/* 函数:CyRecv                       */
/* 参数: s - 必需是非阻塞的套字节                      */
/*      b - 要接收的缓存区指针                     */
/*      l - 要接收的数据长度                          */
/*      toutms - 发送超时(毫秒)                   */
/* 返回: 如果发生错误返回负数,如果发送成功返回>=0,当为0时表示  */ 
/*       数据不能立即接收,需要再次接收                    */
/*       MS_API_ERROR  超时                       */
/*       MS_API_TIMEOUT  错误                  */
/******************************************************/ 
int CyRecv(int s, char *b, int l, int toutms)
{
	int offset = 0;
	int len = l;
	char *buf = b;
	
	while ( len )
	{
		int re = CyRecv_timeout(s, buf+offset, len, toutms);
		if ( re <= 0 ) /* 直接返回错误代码 */
		{
			return re;
		}
		len -= re;
		offset += re;
	}

	return offset;
}

/*******************************************************/
/* 非阻塞超时发送                                                         */
/* 函数:CySend                       */
/* 参数: s - 必需是非阻塞的套字节                      */
/*      b - 要发送的缓存区指针                     */
/*      l - 要发送的数据长度                         */
/*      toutms - 发送超时(毫秒)                  */
/* 返回: 如果发生错误返回负数,如果发送成功返回>=0,当为0时表示  */ 
/*       数据不能立即发送,需要再次发送                   */
/*       MS_API_ERROR  超时                      */
/*       MS_API_TIMEOUT  错误                 */
/******************************************************/ 
int CySend(int s, char *b, int l, int toutms)
{
	int offset = 0;
	int len = l;
	char *buf = b;
	while ( len )
	{
		int re = CySend_timeout(s, buf+offset, len, toutms);
		if ( re <= 0 )
		{
			return re;
		}
		len -= re;
		offset += re;
	
	}

	return offset;
}
/******************************************************************************
 T C P  S O C K E T  O P E R A T I O N S
******************************************************************************/
int tcpsock_init( char *ip, ushort port, int max_conn )
{
	int flags, hsock;
	mtsock_t addr;
	
	hsock = socket(AF_INET, SOCK_STREAM, 0);
	if(hsock < 0) return -1;

	(void) fcntl(hsock, F_SETFD, 1);
	flags = 1;
	(void)setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)&flags, sizeof(flags));
	
	/* Set the listen file descriptor to no-delay / non-blocking mode. */
    set_ndelay( hsock );

	/* bind address and port*/
	bzero(&addr, sizeof(addr));
	addr.sa_in.sin_family = AF_INET;
	if( ip == NULL )
		addr.sa_in.sin_addr.s_addr = htonl(INADDR_ANY);
	else
		addr.sa_in.sin_addr.s_addr = inet_addr(ip);
	addr.sa_in.sin_port = htons(port);
	
	if(bind(hsock, &addr.sa, sizeof(addr)) < 0) 
	{
		close(hsock);
		return -1;
	}
	/*Start a listen going.*/
	if ( listen( hsock, max_conn ) < 0 ) 
	{
		close(hsock);
		return -1;
	}
	
	return hsock;
}

int tcpsock_probe( int hsock, int tminms )
{
	struct timeval tm;
	mtsock_t addr;
	int	hSocket = -1;
	int	addrLen = sizeof(mtsock_t);
	int flags, status = 0;

	status = read_check( hsock, tminms );
	if( status <= 0 ) return status;
	
	bzero(&addr, addrLen);
	hSocket = accept(hsock, &addr.sa, (socklen_t*)&addrLen);
	if( hSocket == -1 ) return -1;

	flags = MAX_SOCKETBUF_SIZ;
	setsockopt(hSocket, SOL_SOCKET, SO_SNDBUF, (char*)&flags, sizeof(flags));
	setsockopt(hSocket, SOL_SOCKET, SO_RCVBUF, (char*)&flags, sizeof(flags));
	tm.tv_sec  = 5;
	tm.tv_usec = 0;
	setsockopt(hSocket, SOL_SOCKET, SO_SNDTIMEO, &tm, sizeof(struct timeval));

	set_ndelay( hSocket );

	return hSocket;
}

// Send to socket
// bytes sent, -1 if failed.
int tcpsock_send( int hsock, char* buf, int len, int tminms )
{
#if 0
	char *ptr = buf;
	int nwrite, nleft, total;
	int status = 0;

	nwrite = 0; nleft = len; total = 0;
	/* Send till end */
	while( nleft > 0) 
	{
		status = write_check( hsock, tminms );
		if (status <= 0) return status;
	
		nwrite = send(hsock, ptr, nleft, 0);
		if(nwrite < 0) 
		{
			printf("tcpsock_send:%d\n",errno);
			if (errno == EINTR || errno == EAGAIN)
				continue;
			else
				return -1;
		}
		nleft -= nwrite; ptr += nwrite; total += nwrite;
	}

	return total;
#endif

	return CySend(hsock, buf, len, tminms);
}

// Receive from socket
// bytes received, -1 if failed.
int tcpsock_recv( int hsock, char* buf, int len, int tminms )
{
#if 0
	char *ptr = buf;
	int nread, nleft, total;
	int status = 0;
	
	status = read_check( hsock, tminms );
	if (status <= 0) return status;

	nread = 0; nleft = len; total = 0;
	/* Receives till end */
	while( nleft > 0) 
	{
		nread = recv(hsock, ptr, nleft, 0);
		if ( nread == -1 ) 
		{
			if (errno == EINTR || errno == EAGAIN)
				continue;
			else return -1;
		}
		// if shutdown by peer
		if (!nread) return -1;
		
		nleft -= nread; ptr += nread; total += nread;
	}

	return total;
#endif

	return CyRecv(hsock, buf, len, tminms);
}

// End of file
