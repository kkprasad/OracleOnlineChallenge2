#include "CMessagingServer.h"
#include "CConfigData.h"
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include<iostream>

int g_iserverPortNumber = 8787;

int main(int argc, char* argv[])
{
		pid_t		l_uprocessId;
		pid_t		l_uparentId;
		pid_t		l_usessionId;
		struct rlimit	l_uresource;
		int		l_iresult = 0;
		int		l_iredirectedFile = 0;
		bool 		l_bsilentMode = false;
		int 		l_iIdx = 0;

		//Read configuration file
		CConfigData l_uconfigData;

		try
		{
				//l_uconfigData.SetConfigurationFileName("/home/veera/prasad/MessagingServer/Source/MessagingServer.cfg");
				l_uconfigData.SetConfigurationFileName("MessagingServer.cfg");
				if( !l_uconfigData.LoadConfigurationData() )
				{
						std::cout<<"Error in loading configuration file. "<<std::endl;
				}
		}
		catch(...)
		{
				std::cout<<"Error in loading configuration file."<<std::endl;
		}
		//If port number is not mentioned in conf file, take default as g_iserverPortNumber
		if(l_uconfigData.GetServerPortNumber() == 0)
		{
				l_uconfigData.SetServerPortNumber(g_iserverPortNumber);
		}
		if(l_uconfigData.GetServerSourceIP() == "")
		{
				l_uconfigData.SetServerSourceIP("localhost");
		}
		if(l_uconfigData.GetSocketType() == InvalidSocketType)
		{
				l_uconfigData.SetSocketType("SynchronousSocket");
		}
		if(l_uconfigData.GetProtocolType() == InvalidSocketProtocol)
		{
				l_uconfigData.SetProtocol("TCPSocket");
		}
		if(l_uconfigData.GetMaxMessageHandlerThreads() == 0)
		{
				l_uconfigData.SetMaxMessageHandlerThreads(5);
		}
		if(l_uconfigData.GetMaxMessageSenderThreads() == 0)
		{
				l_uconfigData.SetMaxMessageSenderThreads(5);
		}
		try
		{
				if(argc > 1)
				{
						int l_iportNumber = 0;
						for( l_iIdx = 1; l_iIdx < argc; l_iIdx++) {
								//	if ( strlen( argv[l_iIdx]) != 2 ) break;
								if ( argv[l_iIdx][0] != '-' ) break;
								else if ( argv[l_iIdx][1] == 'p' ) 
								{
										if(strlen(argv[l_iIdx]) > 2)
										{
												l_iportNumber = atol(argv[l_iIdx]+2);
										}
										else
										{
												if(l_iIdx < argc-1)
												{
														l_iportNumber = atol(argv[++l_iIdx]);
												}
										}
										l_uconfigData.SetServerPortNumber( l_iportNumber);
								}
								else break;
						}
						if ( l_iIdx < argc | l_iportNumber == 0 ) {
								std::cout<<std::endl<<"Invalid parameter: "<<argv[l_iIdx];
								std::cout<<std::endl<<"Usage: "<<argv[0]<<" [-p]";
								std::cout<<std::endl<<"Options: ";
								std::cout<<std::endl<<"    -p: Port Number";
								return EXIT_FAILURE;
						}
				}
		}
		catch(...)
		{
				std::cout<<"Error in processing command line arguments"<<std::endl;
		}
		std::cout<<"Messaging server Listening port="<<l_uconfigData.GetServerPortNumber()<<std::endl;
		if ( l_bsilentMode )
		{
				// Fork off the parent process
				l_uprocessId = fork();
				if ( l_uprocessId < 0 ) {
						exit(EXIT_FAILURE);
				}
				// If we got a good PID, then we can exit the parent process.
				if ( l_uprocessId > 0 ) {
						exit(EXIT_SUCCESS);
				}

				// Change the file mode mask
				umask(0);

				// Close all opened resources
				getrlimit(RLIMIT_NOFILE, &l_uresource);
				if ( l_uresource.rlim_max == 0 ) {
						exit(EXIT_FAILURE);
				}

				for ( l_iresult = 0; l_iresult < l_uresource.rlim_max; l_iresult++ ) close(l_iresult);
				close(STDIN_FILENO);
				close(STDOUT_FILENO);
				close(STDERR_FILENO);

				l_iredirectedFile = open("/dev/null", 2);
				// li_redirectedFile = open("/home/vinod/prov/src/GlobalClient/1.log", O_CREAT|O_SYNC);
				dup2(l_iredirectedFile, 1);
				dup2(l_iredirectedFile, 2);
				dup2(l_iredirectedFile, 3);

				// Create a new SID for the child process
				l_uprocessId = setsid();
				if ( l_uprocessId < 0 ) exit(EXIT_FAILURE);

				// Change the current working directory
				if ( chdir("/") < 0 ) exit(EXIT_FAILURE);
		}
		CMessagingServer * l_upMessagingServer = NULL;
		l_upMessagingServer = new CMessagingServer;

		l_upMessagingServer->SetPortNumber(l_uconfigData.GetServerPortNumber());
		l_upMessagingServer->SetProtocol(l_uconfigData.GetProtocolType());
		l_upMessagingServer->SetSocketType(l_uconfigData.GetSocketType());
		l_upMessagingServer->SetSourceIP(l_uconfigData.GetServerSourceIP());
		l_upMessagingServer->SetServerConnectionQueueSize(30);
		l_upMessagingServer->SetMaxMessageHandlerThreads(l_uconfigData.GetMaxMessageHandlerThreads());
		l_upMessagingServer->SetMaxMessageSenderThreads(l_uconfigData.GetMaxMessageSenderThreads());

		l_upMessagingServer->StartServer();
		l_upMessagingServer->StartThreads();

		while(1)
		{
				sleep(5);
		}
}
