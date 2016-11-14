#ifndef	__NETSOCK_H
#define	__NETSOCK_H

#include <sys/socket.h>
#include <netinet/in.h>

/* Default socket buffer size */
#define MAX_SOCKETBUF_SIZ  32768

// Socket address
typedef union mtsock_t
{
    struct sockaddr sa;
    struct sockaddr_in sa_in;
} mtsock_t;

/* function declarations */

extern int tcpsock_init( char *ip, ushort port, int max_conn );
extern int tcpsock_probe( int hsock, int tminms );
extern int tcpsock_send( int fd, char* buf, int len, int tminms );
extern int tcpsock_recv( int fd, char* buf, int len, int tminms );

#endif

