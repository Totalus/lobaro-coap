
#ifndef COAP_INTERFACE_POSIX_H
#define COAP_INTERFACE_POSIX_H

#include "../../liblobaro_coap.h"

SocketHandle_t CoAP_Posix_CreateSocket();

bool CoAP_Posix_SendDatagram(SocketHandle_t *socketHandle, NetPacket_t *pckt);

#endif
