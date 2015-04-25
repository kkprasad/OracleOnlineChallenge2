#include "CSynchronousSocketFactory.h"

CBasicSocket* CSynchronousSocketFactory::MakeSocketObject(int a_iprotocol)
{
	switch (a_iprotocol)
	{
		case IPPROTO_TCP:
			return new CTCPSocket;
			break;
		default:
			return NULL;
	};
}
