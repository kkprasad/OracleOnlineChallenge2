#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/types.h>

enum SocketProtocol
{
        TCPSocket = IPPROTO_TCP,
        UDPSocket = IPPROTO_UDP,
        SCTPSocket = IPPROTO_SCTP,
        InvalidSocketProtocol = -1
};
enum SocketType
{
        SynchronousSocket = 1,
        AsynchronousSocket = 2,
        InvalidSocketType = -1
};

#endif
