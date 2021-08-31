/*
	Posix socket interface for lobaro-coap
*/

#include "../../coap.h"
#include "socket_porting.h" // or <socket.h>


// Send a packet to the network interface
bool CoAP_Posix_SendDatagram(SocketHandle_t socketHandle, NetPacket_t *pckt)
{
	//INFO("CoAP_Posix_SendDatagram called\n");
	int sock = (int)socketHandle;

	// Format the endpoint info to the right structure
	struct sockaddr addr;
	size_t sockaddrSize;
	if(pckt->remoteEp.NetType == IPV4)
	{
		struct sockaddr_in *remote = &addr;
		remote->sin_family = AF_INET;
		remote->sin_port = htons(pckt->remoteEp.NetPort);
		for(uint8_t i = 0; i < 4; i++)
			remote->sin_addr.s4_addr[i] = pckt->remoteEp.NetAddr.IPv4.u8[i];
		sockaddrSize = sizeof(struct sockaddr_in);
	}
	else if(pckt->remoteEp.NetType == IPV6)
	{
		// WARNING: IPv6 not tested

		struct sockaddr_in6 *remote = &addr;
		remote->sin6_family = AF_INET6;
		remote->sin6_port = htons(pckt->remoteEp.NetPort);
		for(uint8_t i = 0; i < 16; i++)
			remote->sin6_addr.s6_addr[i] = pckt->remoteEp.NetAddr.IPv6.u8[i];
		remote->sin6_scope_id = 0; // Don't know if that's a good value, normaly mapped to the interface index
		sockaddrSize = sizeof(struct sockaddr_in6);
	}
	else
	{
		ERROR("Unsupported NetType : %d\n", pckt->remoteEp.NetType);
		return false;
	}

	int ret = sendto(sock, pckt->pData, pckt->size, 0, &addr, sockaddrSize);

	if(ret < 0)
		ERROR("sendto() returned %d (errno = %d)\n", ret, errno);

	return ret > 0;
}

// Create a CoAP socket that can be used with the CoAP library
// Returns true on success, false if the socket could not be created
bool CoAP_Posix_CreateSocket(SocketHandle_t *handle, NetInterfaceType_t type)
{
	if(type == IPV4) {

		// Create the actual socket
		int posixSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if(posixSocket < 0) {
			ERROR("Could not create socket, errno = %d", errno);
			return false;
		}

		// Allocate a new CoAP_Socket_t space
		CoAP_Socket_t *newSocket = AllocSocket();

		if(newSocket == NULL) {
			ERROR("Could not allocate memory for new socket");
			close(socket);
			return false;
		}

		newSocket->Handle = posixSocket;
		newSocket->Tx = CoAP_Posix_SendDatagram;
		newSocket->Alive = true;
		*handle = posixSocket;
		return true;
	}
	else {
		ERROR("Unsupported net type %d", type);
	}
	
	return false;
}


void CoAP_Posix_CloseSocket(SocketHandle_t handle)
{
	CoAP_Socket_t* coapSocket = RetrieveSocket(handle);

	if(coapSocket == NULL)
		return;

	close(coapSocket->Handle);
	coapSocket->Alive = false;
}



