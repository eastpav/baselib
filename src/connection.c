#include <stdio.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "miscutil.h"
#include "connection.h"
#include "netsock.h"
#include <modids.h>

// Connection object definition
typedef struct connection_obj
{
	int        hSock;
	ComMode     mode;
	ComProtocol prot;
}ConnectionObj;

// Create Connection object.
// RETURNS: Handle to Connection object, NULL if failed.
void* Connection_create (ComMode mode, ComProtocol prot, void* params )
{
	ConnectionObj *hndl = NULL;

	hndl = (ConnectionObj*)malloc(sizeof(ConnectionObj));
	if(hndl == NULL) return NULL;

	memset(hndl, 0, sizeof(ConnectionObj));

	if (mode == COM_MODE_NETWORK)
	{
		if (prot == COM_PROT_TCPIP4)
		{
			// Initializes network connection.
			hndl->hSock = socket(AF_INET, SOCK_STREAM, 0);
			if (hndl->hSock < 0)
			{
				free((char*)hndl);
				return NULL;
			}

			// Set to delay mode
		//	(void) fcntl( hndl->hSock, F_SETFL, fcntl( hndl->hSock, F_GETFL, 0 ) & ~ (int) O_NDELAY );
			//设置非阻塞模式，设置超时
			{
			int rttt = 2;
			rttt = fcntl( hndl->hSock, F_SETFL, fcntl( hndl->hSock, F_GETFL, 0 ) & (int) O_NONBLOCK);
			struct timeval tm;
			tm.tv_sec  = 0;
			tm.tv_usec = 50000;
			rttt = setsockopt(hndl->hSock, SOL_SOCKET, SO_SNDTIMEO, &tm, sizeof(struct timeval));
			}
			hndl->mode = mode;
			hndl->prot = prot;
			
			return (void*)hndl;
		}
	}

	free((char*)hndl);

	return NULL;
}

// Delete Connection object.
// RETURNS: 0 - OK, -1 if failed.
int Connection_delete (void* handle )
{
	ConnectionObj* hndl = (ConnectionObj*)handle;

	if (hndl == NULL) 
	{
		return MOD_ERRCODE(CONNECTION_MODID, EMOD_INVALID);
	}

	if (hndl->mode == COM_MODE_NETWORK)
	{
		if (hndl->prot == COM_PROT_TCPIP4)
		{
			close(hndl->hSock);
		}
	}
	else
	{
		return MOD_ERRCODE(CONNECTION_MODID, EMOD_NOIMPL);
	}

	free((char*)hndl);

	return 0;
}

// Send specific data to remote server.
// RETURNS: sent data in bytes, 0 - timeout, -1 if failed.
int Connection_send (void* handle, char* pBuffer, int len, int tminms )
{
	ConnectionObj* hndl = (ConnectionObj*)handle;

	if (hndl == NULL) 
	{
		return MOD_ERRCODE(CONNECTION_MODID, EMOD_INVALID);
	}

	if (hndl->mode == COM_MODE_NETWORK)
	{
		if (hndl->prot == COM_PROT_TCPIP4)
		{
			return tcpsock_send(hndl->hSock, pBuffer, len, tminms);
		}
	}
	else
	{
		return MOD_ERRCODE(CONNECTION_MODID, EMOD_NOIMPL);
	}
	
	return 0;
}

// Receives specific data from remote server.
// RETURNS: ReceiveD data in bytes, 0 - timeout, -1 if failed.
int Connection_recv (void* handle, char* pBuffer, int len, int tminms )
{
	ConnectionObj* hndl = (ConnectionObj*)handle;

	if (hndl == NULL) 
	{
		return MOD_ERRCODE(CONNECTION_MODID, EMOD_INVALID);
	}

	if (hndl->mode == COM_MODE_NETWORK)
	{
		if (hndl->prot == COM_PROT_TCPIP4)
		{
			return tcpsock_recv(hndl->hSock, pBuffer, len, tminms);
		}
	}
	else
	{
		return MOD_ERRCODE(CONNECTION_MODID, EMOD_NOIMPL);
	}
	
	return -1;
}

// Connect to remote server
// RETURNS: 1 - OK, 0 - TMOUT, -1 - ERROR.
int Connection_doWork (void* handle, NetConnPoint* pConn, int tminms )
{
	mtsock_t addr;
	ConnectionObj* hndl = (ConnectionObj*)handle;
	int status = 0;

	if (hndl == NULL) 
	{
		return MOD_ERRCODE(CONNECTION_MODID, EMOD_INVALID);
	}

	bzero(&addr, sizeof(addr));
	
	/* fill server end-point information */
	addr.sa_in.sin_family = AF_INET;
	addr.sa_in.sin_addr.s_addr = pConn->ifaddr;
	addr.sa_in.sin_port = htons(pConn->port);

	// Connect to remote server async
	if(connect( hndl->hSock, (struct sockaddr*)&addr, 
		sizeof(struct sockaddr) ) == -1) 
	{
		if (errno == EINTR || 
			errno == EINPROGRESS || 
			errno == EALREADY)
			return 0;
		else return MOD_ERRCODE(CONNECTION_MODID, errno);
	}

	// Wait for specific time in ms.
	status = write_check(hndl->hSock, tminms);
	if (status <= 0) return status;

	pConn->isOn = 1;
	
	return 1;
}

// End of file
