/* server.c - Communication server source file */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>
#include <sys/select.h>
#include "server.h"
#include "netsock.h"
#include <modids.h>

// server object definition
typedef struct server_obj
{
	int        hSock;
	ComMode     mode;
	ComProtocol prot;
}ServerObj;

// Create a server object, Only IPv4 is supported.
// RETURNS: Handle to server object, NULL if failed.
void* Server_create (ComMode mode, ComProtocol prot, void* params )
{
	ServerObj *hndl = NULL;

	// Create server.
	hndl = (ServerObj*)malloc(sizeof(ServerObj));
	if(hndl == NULL) return NULL;

	memset(hndl, 0, sizeof(ServerObj));

	if (mode == COM_MODE_NETWORK)
	{
		if (prot == COM_PROT_TCPIP4)
		{
			// Initializes network server.
			NetServerAttribute *pAttr = (NetServerAttribute*)params;
			hndl->hSock = tcpsock_init( NULL, pAttr->port, pAttr->maxCount );
			if (hndl->hSock < 0)
			{
				free((char*)hndl);
				return NULL;
			}
			hndl->mode = mode;
			hndl->prot = prot;

			return (void*)hndl;
		}
	}

	free((char*)hndl);

	return NULL;
}

// Delete server object
// RETURNS: 0 - OK, -1 if failed.
int Server_delete (void* handle )
{
	ServerObj* hndl = (ServerObj*)handle;

	if (hndl == NULL) 
	{
		return MOD_ERRCODE(SERVER_MODID, EMOD_INVALID);
	}

	if (hndl->mode == COM_MODE_NETWORK)
	{
		if (hndl->prot == COM_PROT_TCPIP4)
		{
			close(hndl->hSock);
		}
		else
		{
			return MOD_ERRCODE(SERVER_MODID, EMOD_NOIMPL);
		}
	}
	else
	{
		return MOD_ERRCODE(SERVER_MODID, EMOD_NOIMPL);
	}

	free((char*)hndl);

	return 0;
}

// Send specific data to server object
// RETURNS: sent data in bytes, 0 - timeout, -1 if failed.
int Server_send (void* handle, void* hDst, char* pBuffer, int len, int tminms )
{
	ServerObj* hndl = (ServerObj*)handle;

	if (hndl == NULL) 
	{
		return MOD_ERRCODE(SERVER_MODID, EMOD_INVALID);
	}

	if (hndl->mode == COM_MODE_NETWORK)
	{
		if (hndl->prot == COM_PROT_TCPIP4)
		{
			return tcpsock_send((int)hDst, pBuffer, len, tminms);
		}
		else
		{
			return MOD_ERRCODE(SERVER_MODID, EMOD_NOIMPL);
		}
	}
	else
	{
		return MOD_ERRCODE(SERVER_MODID, EMOD_NOIMPL);
	}
	
	return 0;
}

// Receives specific data from server object
// RETURNS: sent data in bytes, 0 - timeout, -1 if failed.
int Server_recv (void* handle, void* hSrc, char* pBuffer, int len, int tminms )
{
	ServerObj* hndl = (ServerObj*)handle;

	if (hndl == NULL) 
	{
		return MOD_ERRCODE(SERVER_MODID, EMOD_INVALID);
	}

	if (hndl->mode == COM_MODE_NETWORK)
	{
		if (hndl->prot == COM_PROT_TCPIP4)
		{
			return tcpsock_recv((int)hSrc, pBuffer, len, tminms);
		}
		else
		{
			return MOD_ERRCODE(SERVER_MODID, EMOD_NOIMPL);
		}
	}
	else
	{
		return MOD_ERRCODE(SERVER_MODID, EMOD_NOIMPL);
	}
	
	return 0;
}

// Probe server object
// RETURNS: 1 - OK, 0 - timeout, -1 if failed.
int Server_probe (void* handle, int tminms )
{
	ServerObj* hndl = (ServerObj*)handle;

	if (hndl == NULL) 
	{
		return MOD_ERRCODE(SERVER_MODID, EMOD_INVALID);
	}

	if (hndl->mode == COM_MODE_NETWORK)
	{
		if (hndl->prot == COM_PROT_TCPIP4)
		{
			return tcpsock_probe(hndl->hSock, tminms );
		}
		else
		{
			return MOD_ERRCODE(SERVER_MODID, EMOD_NOIMPL);
		}
	}
	else
	{
		return MOD_ERRCODE(SERVER_MODID, EMOD_NOIMPL);
	}
	
	return 0;
}

// End of file
