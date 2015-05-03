#include "CMessagingServer.h"
#include "CConfigData.h"
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include<iostream>
#include <signal.h>
#include <stdlib.h>

int g_iserverPortNumber = -1;
std::string g_sConfigurationFileName;
std::string g_sLogDirectory ;
std::string g_sPIDFile;
CMessagingServer * g_upMessagingServer = NULL;


void ExecuteExitProcedures(int);
void RemovePIDFile();
bool WritePIDFile();


int main(int argc, char* argv[])
{
	pid_t		l_uprocessId;
	pid_t		l_uparentId;
	pid_t		l_usessionId;
	struct rlimit	l_uresource;
	int		l_iresult = 0;
	int		l_iredirectedFile = 0;
	bool 		l_bsilentMode = true;
	int 		l_iIdx = 0;
	CConfigData 	l_uconfigData;
	std::string	l_sHomeDirectory;
	signal(SIGINT, ExecuteExitProcedures);
	
	try
	{
		if(argc > 1)
		{
			int l_iportNumber = 0;
			for( l_iIdx = 1; l_iIdx < argc; l_iIdx++) {
				if ( argv[l_iIdx][0] != '-' ) 
				{
					break;
				}
				else if ( argv[l_iIdx][1] == 'p' ) 
				{
					if(strlen(argv[l_iIdx]) > 2)
					{
						g_iserverPortNumber = atol(argv[l_iIdx]+2);
					}
					else
					{
						if(l_iIdx < argc-1)
						{
							g_iserverPortNumber = atol(argv[++l_iIdx]);
						}
					}
				}
				else if ( argv[l_iIdx][1] == 'c' ) 
				{
					if(strlen(argv[l_iIdx]) > 2)
					{
						g_sConfigurationFileName = argv[l_iIdx]+2;
						//strcpy(g_cpConfigurationFileName,argv[l_iIdx]+2);
					}
					else
					{
						if(l_iIdx < argc-1)
						{
							g_sConfigurationFileName = argv[++l_iIdx];
							//strcpy(g_cpConfigurationFileName,argv[++l_iIdx]);
						}
					}
					l_uconfigData.SetConfigurationFileName( g_sConfigurationFileName);
				}
				else if ( argv[l_iIdx][1] == 'l' ) 
				{
					if(strlen(argv[l_iIdx]) > 2)
					{
						g_sLogDirectory = argv[l_iIdx]+2;
						//strcpy(g_cpConfigurationFileName,argv[l_iIdx]+2);
					}
					else
					{
						if(l_iIdx < argc-1)
						{
							g_sLogDirectory = argv[++l_iIdx];
							//strcpy(g_cpConfigurationFileName,argv[++l_iIdx]);
						}
					}
				}
				else if ( argv[l_iIdx][1] == 't' ) 
				{
					l_bsilentMode = false;
				}
				else break;
			}
			if ( l_iIdx < argc) {
				std::cout<<std::endl<<"Invalid parameter: "<<argv[l_iIdx];
				std::cout<<std::endl<<"Usage: "<<argv[0]<<" [-p <port number>] [-c <configuration file>] [-l <log directory>] [-t]";
				std::cout<<std::endl<<"Options: ";
				std::cout<<std::endl<<"\t-p: Port Number\n\t-c: Configuration file\n\t-l: Log Directory\n\t-t : Testing Mode";
				return EXIT_FAILURE;
			}
		}
	}
	catch(...)
	{
		CFileHandler::GetInstance().WriteToSysLog("Unknown Exception Raised in main :Error in processing command line arguments: ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
	}
	l_sHomeDirectory = "";
	l_sHomeDirectory = getenv("MSGSERVER_HOME");
	if(l_sHomeDirectory.length() == 0)
	{
		CFileHandler::GetInstance().WriteToSysLog("Environment variable MSGSERVER_HOME is not set. Messaging Server Exiting.");
		exit(EXIT_FAILURE);
	}
	g_sPIDFile = l_sHomeDirectory;
	g_sPIDFile += "var/run/msgserverd.pid";

	CFileHandler::SetLogFileDirectory("/tmp/");
	//Read configuration file
	try
	{
		//l_uconfigData.SetConfigurationFileName("/home/veera/prasad/MessagingServer/Source/MessagingServer.cfg");
		//l_uconfigData.SetConfigurationFileName("/etc/MsgServer.cfg");
		//l_uconfigData.SetConfigurationFileName("/home/veera/prasad/SocketPrograms/MessagingServer/Source/MsgServer.cfg");
		if(l_uconfigData.GetConfigurationFileName().length() == 0)
		{
			g_sConfigurationFileName = "/opt/msgserverd/etc/msgserverd.conf";
			l_uconfigData.SetConfigurationFileName(g_sConfigurationFileName);
		}
		if( !l_uconfigData.LoadConfigurationData() )
		{
			CFileHandler::GetInstance().WriteToSysLog("Error in loading configuration file");
		}
	}
	catch(CException a_uException)
	{
		CFileHandler::GetInstance().WriteToSysLog("Exception Raised while loading configuration file:%s",a_uException.what());
	}
	catch(...)
	{
		CFileHandler::GetInstance().WriteToSysLog("Unknown exception raised while loading configuration file:ErrorNo:%d, ErrorString:%s",errno,strerror(errno));
	}
	try
	{
		if(g_iserverPortNumber > 0)
		{
			l_uconfigData.SetServerPortNumber(g_iserverPortNumber);
		}
		else if(l_uconfigData.GetServerPortNumber() == 0)
		{
			l_uconfigData.SetServerPortNumber(8787);
		}
		if(l_uconfigData.GetServerSourceIP().length() == 0)
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
		if(g_sLogDirectory.length()>0)
		{
			l_uconfigData.SetLogFileDirectory(g_sLogDirectory);
		}
		else if(l_uconfigData.GetLogFileDirectory().length() == 0)
		{
			l_uconfigData.SetLogFileDirectory("/tmp");
		}
	}
	catch(...)
	{
		CFileHandler::GetInstance().WriteToSysLog("Error in assigning default values to configuration parameters");
	}

	CFileHandler::SetLogFileDirectory(l_uconfigData.GetLogFileDirectory());
	try
	{
		if ( l_bsilentMode )
		{
			// Fork off the parent process
			l_uprocessId = fork();
			if ( l_uprocessId < 0 ) {
				exit(EXIT_FAILURE);
			}
			// If we got a good PID, then we can exit the parent process.
			if ( l_uprocessId > 0 ) {
				sleep(3); 
				exit(EXIT_SUCCESS);
			}
			// Change the file mode mask
			umask(0);

			// Close all opened resources
			getrlimit(RLIMIT_NOFILE, &l_uresource);
			if ( l_uresource.rlim_max == 0 ) {
				exit(EXIT_FAILURE);
			}

			/*for ( l_iresult = 0; l_iresult < l_uresource.rlim_max; l_iresult++ ) close(l_iresult);*/
			
			close(STDIN_FILENO);
			close(STDOUT_FILENO);
			close(STDERR_FILENO);

			l_iredirectedFile = open("/dev/null", 2);
			dup2(l_iredirectedFile, 1);
			dup2(l_iredirectedFile, 2);
			dup2(l_iredirectedFile, 3);
			
			// Create a new SID for the child process
			l_uprocessId = setsid();
			if ( l_uprocessId < 0 ) exit(EXIT_FAILURE);

			// Change the current working directory
			if ( chdir("/") < 0 ) exit(EXIT_FAILURE);
			//Write to PID file
			if(! WritePIDFile() )
			{
				CFileHandler::GetInstance().WriteToSysLog("WritePIDFile returns FALSE. Messaging Server Exiting.");
				
			}

		}
	}
	catch(...)
	{
		CFileHandler::GetInstance().WriteToFile(400,"Unknown Exception Raised in main :Error in making application as daemon: ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		CFileHandler::GetInstance().WriteToSysLog("Unknown Exception Raised in main :Error in making application as daemon: ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		exit(EXIT_FAILURE);
	}
	try
	{
		g_upMessagingServer = new CMessagingServer;
		if(g_upMessagingServer == NULL)
		{

			CFileHandler::GetInstance().WriteToFile(400,"Error in allocating memory for CMessagingServer: ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
			CFileHandler::GetInstance().WriteToSysLog("Error in allocating memory for CMessagingServer: ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
			exit(EXIT_FAILURE);
		}

		g_upMessagingServer->SetPortNumber(l_uconfigData.GetServerPortNumber());
		g_upMessagingServer->SetProtocol(l_uconfigData.GetProtocolType());
		g_upMessagingServer->SetSocketType(l_uconfigData.GetSocketType());
		g_upMessagingServer->SetSourceIP(l_uconfigData.GetServerSourceIP());
		g_upMessagingServer->SetServerConnectionQueueSize(30);
		g_upMessagingServer->SetMaxMessageHandlerThreads(l_uconfigData.GetMaxMessageHandlerThreads());
		g_upMessagingServer->SetMaxMessageSenderThreads(l_uconfigData.GetMaxMessageSenderThreads());

		CFileHandler::GetInstance().WriteToFile(400,"Starting Server...\n");
		CFileHandler::GetInstance().WriteToFile(400,"Messaging server Listening port=%d",l_uconfigData.GetServerPortNumber());
		CFileHandler::GetInstance().WriteToSysLog("Messaging server Listening port=%d",l_uconfigData.GetServerPortNumber());

		g_upMessagingServer->StartServer();
		CFileHandler::GetInstance().WriteToFile(400,"Starting Threads...\n");
		g_upMessagingServer->StartThreads();
		CFileHandler::GetInstance().WriteToFile(400,"Started Threads...\n");
		CFileHandler::GetInstance().WriteToSysLog("Messaging Server Started...");

		while(g_upMessagingServer->GetServerRunningStatus())
		{
			sleep(5);
		}
	}
	catch(CException a_uException)
	{
		CFileHandler::GetInstance().WriteToFile(400,"Unknown Exception Raised in main:Starting Message Handling Server :%s",a_uException.what());
		ExecuteExitProcedures(EXIT_FAILURE);
	}
	catch(...)
	{
		CFileHandler::GetInstance().WriteToFile(400,"Unknown Exception Raised in main:Starting Message Handling Server ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		ExecuteExitProcedures(EXIT_FAILURE);
	}
}

void ExecuteExitProcedures(int a_iSignalNumber)
{
	if(g_upMessagingServer != NULL)
	{
		g_upMessagingServer->StopThreads();
		usleep(1000);
		g_upMessagingServer->StopServer();
		usleep(1000);
		delete g_upMessagingServer;
	}
	CFileHandler::ClearFilePointerMap();
	RemovePIDFile();
	exit(a_iSignalNumber);
}
bool WritePIDFile()
{
	try
	{

		FILE *l_FFilePointer = fopen(g_sPIDFile.c_str(), "w");
		if (l_FFilePointer == NULL) {
			CFileHandler::GetInstance().WriteToSysLog("Unable to open PID File");
			return false;
		}
		else
		{
			CFileHandler::GetInstance().WriteToSysLog("PID=%ld",getpid());
			fprintf(l_FFilePointer,"%ld",getpid());
			fclose(l_FFilePointer);
			return true;
		}
	}
	catch(...)
	{
		CFileHandler::GetInstance().WriteToSysLog("Error in writing to PID File");
		return false;
	}
}
void RemovePIDFile()
{
	std::string l_sCommand;
	l_sCommand = "rm -f ";
	l_sCommand += g_sPIDFile;
	system(l_sCommand.c_str());
}
