#include "CMessagingServer.h"

int main()
{
	CMessagingServer * l_upMessagingServer = NULL;
	l_upMessagingServer = new CMessagingServer;

	l_upMessagingServer->SetPortNumber(8787);
	l_upMessagingServer->SetProtocol(TCPSocket);
	l_upMessagingServer->SetSocketType( SynchronousSocket);
	l_upMessagingServer->SetServerConnectionQueueSize(10);
	l_upMessagingServer->SetMaxMessageHandlerThreads(10);
	l_upMessagingServer->SetSourceIP("192.168.1.250");

	l_upMessagingServer->StartThreads();
	l_upMessagingServer->StartServer();

	while(1)
	{
		sleep(5);
	}
}
