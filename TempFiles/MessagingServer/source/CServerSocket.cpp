#include "CServerSocket.h"

int CServerSocket::m_iSourcePortNumber = -1;
int CServerSocket::m_iServerConnectionQueueSize = 20;
std::string CServerSocket::m_sSourceIP = "";
SocketProtocol CServerSocket::m_uProtocol = InvalidSocketProtocol;
SocketType CServerSocket::m_uSocketType = InvalidSocketType;
pthread_t CServerSocket::m_uListenThreadId = -1;
bool CServerSocket::m_bServerRunningStatus = false;

CServerSocket::CServerSocket()
{
	this->m_upServerSocketObject = NULL;
}

CServerSocket::~CServerSocket()
{
	if(this->m_upServerSocketObject!=NULL)
	{
		delete this->m_upServerSocketObject;
		this->m_upServerSocketObject = NULL;
	}
}
void CServerSocket::SetServerConnectionQueueSize(int a_iServerConnectionQueueSize)
{
	CServerSocket::m_iServerConnectionQueueSize = a_iServerConnectionQueueSize;
}

int CServerSocket::GetServerConnectionQueueSize()
{
	return CServerSocket::m_iServerConnectionQueueSize;
}

void CServerSocket::SetSourceIP(std::string a_ssourceIP)
{
	CServerSocket::m_sSourceIP = a_ssourceIP;
}
std::string CServerSocket::GetSourceIP()
{
	return CServerSocket::m_sSourceIP;
}
void CServerSocket::SetServerRunningStatus(bool a_bServerRunningStatus)
{
	CServerSocket::m_bServerRunningStatus = a_bServerRunningStatus;
}
bool CServerSocket::GetServerRunningStatus()
{
	return CServerSocket::m_bServerRunningStatus;
}
void CServerSocket::SetPortNumber(int a_iPortNumber)
{
	CServerSocket::m_iSourcePortNumber = a_iPortNumber;
}

int CServerSocket::GetPortNumber()
{
	return CServerSocket::m_iSourcePortNumber;
}

void CServerSocket::SetProtocol(SocketProtocol a_uProtocol)
{
	CServerSocket::m_uProtocol = a_uProtocol;
}

SocketProtocol CServerSocket::GetProtocol()
{
	return CServerSocket::m_uProtocol;
}

void CServerSocket::SetSocketType(SocketType a_uSocketType)
{
	CServerSocket::m_uSocketType = a_uSocketType;
}

SocketType CServerSocket::GetSocketType()
{
	return CServerSocket::m_uSocketType;
}

bool   CServerSocket::MakeServerSocketInstance(bool a_bNewSocketFlag)
{
	return this->MakeServerSocketInstance(a_bNewSocketFlag,0);
}
bool   CServerSocket::MakeServerSocketInstance(bool a_bNewSocketFlag,int a_iSocketFD)
{
	CBasicSocket* l_upSocketObject = NULL;

	CSocketFactory *l_upSocketFactory = NULL;
	//Create the socket Factory
	try
	{
		switch(CServerSocket::GetSocketType())
		{
			case SynchronousSocket:
				l_upSocketFactory = new CSynchronousSocketFactory;
				if( l_upSocketFactory == NULL)
				{
					CException l_uException("Memory allocation for CSynchronousSocketFactory FAILED");
					throw l_uException;
				}
				break;
			default:
				break;
		};
	}
	catch(CException a_uException)
	{
		CFileHandler::GetInstance().WriteToFile(600,"Exception Raised in CServerSocket::MakeServerSocketInstance:%s",a_uException.what());
		return false;
	}
	catch(...)
	{
		CFileHandler::GetInstance().WriteToFile(600,"Unknown Exception Raised in CServerSocket:MakeServerSocketInstance:Error in creating Socket Factory:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		return false;
	}	
	// Create the Socket Object
	try
	{
		l_upSocketObject = l_upSocketFactory->MakeSocketObject(CServerSocket::m_uProtocol);	
		if( l_upSocketObject == NULL)
		{
			CException l_uException("MakeSocketObject Returned NULL");
			throw l_uException;
		}
		delete l_upSocketFactory;
	}
	catch(CException a_uException)
	{
		if(l_upSocketFactory != NULL)
		{
			delete l_upSocketFactory;
		}
		CFileHandler::GetInstance().WriteToFile(600,"Exception Raised in CServerSocket::MakeServerSocketInstance:%s",a_uException.what());
		return false;
	}

	catch(...)
	{
		if(l_upSocketFactory != NULL)
		{
			delete l_upSocketFactory;
		}
		CFileHandler::GetInstance().WriteToFile(600,"Unknown Exception Raised in CServerSocket:MakeServerSocketInstance:Error in creating Socket Instance:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		return false;
	}
	try
	{
		this->SetSocketInstance(l_upSocketObject);
		if(a_bNewSocketFlag)
		{
			if (! this->GetSocketInstance()->Create() )
			{
				return false;
			}
			this->SetServerRunningStatus(true);
		}
		else
		{
			this->GetSocketInstance()->SetSocketFD(a_iSocketFD);
		}
		this->GetSocketInstance()->SetConnectionStatus(true);
	}
	catch(...)
	{
		CFileHandler::GetInstance().WriteToFile(600,"Unknown Exception Raised in CServerSocket:MakeServerSocketInstance:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		return false;
	}
	return true;
}

CBasicSocket* CServerSocket::GetSocketInstance()
{
	return this->m_upServerSocketObject;
}

void CServerSocket::UnSetSocketInstance()
{
	try
	{
		if( this->m_upServerSocketObject != NULL)
		{
			this->m_upServerSocketObject->Close();
			sleep(1);
			delete this->m_upServerSocketObject;
			this->m_upServerSocketObject = NULL;
		}
	}
	catch(...)
	{
		CFileHandler::GetInstance().WriteToFile(600,"Unknown Exception Raised in CServerSocket:UnSocketInstance:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
	}
}
void CServerSocket::SetSocketInstance(CBasicSocket* a_upSocketObject)
{
	this->m_upServerSocketObject = a_upSocketObject;
}
bool CServerSocket::SendMessage(std::string a_sData, int& a_iMessageLength)
{
	if(this->GetSocketInstance()->GetConnectionStatus())
	{
		return this->GetSocketInstance()->Send(a_sData, a_iMessageLength);
	}
	else
	{
		return false;
	}
}

bool CServerSocket::ReceiveMessage(std::string& a_sData, int& a_iReceivedDataLength)
{
	if(this->GetSocketInstance()->GetConnectionStatus())
        {
		return this->GetSocketInstance()->Receive(a_sData,a_iReceivedDataLength);
	}
	else
        {
                return false;
        }
}

void CServerSocket::ForceDisconnect(int a_iSocketFD)
{
	try
	{
		CBasicSocket::ForceClose(a_iSocketFD);
		CBasicSocket::ForceShutdown(a_iSocketFD);
	}
	catch(...)
	{
		CFileHandler::GetInstance().WriteToFile(600,"Unknown Exception Raised in CServerSocket::ForceDisconnect:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
	}
}
bool CServerSocket::DisconnectClient()
{
	try
	{
		this->GetSocketInstance()->Close();
		this->GetSocketInstance()->Shutdown();
		return true;
	}
	catch(...)
	{
		CFileHandler::GetInstance().WriteToFile(600,"Unknown Exception Raised in CServerSocket::DisconnectClient:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		return false;
	}
}

void CServerSocket::OnAccept(int a_iSocketFD)
{
}

void CServerSocket::OnMessageReceive(std::string a_sReceivedMessage, int a_iReceivedMessageLength)
{
}

void CServerSocket::OnDisconnect(int a_iSocketFD)
{
}
bool CServerSocket::Bind()
{
	try
	{
		if( this->GetSocketInstance()->Bind(CServerSocket::m_sSourceIP,CServerSocket::m_iSourcePortNumber) )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	catch(...)
	{
                CFileHandler::GetInstance().WriteToFile(600,"Unknown Exception Raised in CServerSocket::Bind:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		return false;
	}

}
int CServerSocket::Accept()
{
	return this->GetSocketInstance()->Accept();
}
bool CServerSocket::Listen()
{
	return this->GetSocketInstance()->Listen(CServerSocket::m_iServerConnectionQueueSize);
}

void* CServerSocket::ThreadListenConnections(void* a_upArguments)
{
	CServerSocket *l_upCurrentObject = (CServerSocket*)a_upArguments;
	try
	{
		//Bind to source IP and Port
		if(!l_upCurrentObject->Bind())
		{
			CException l_uException("Bind Returned FALSE");
			throw l_uException;
		}

		//Listen to the socket
		if(!l_upCurrentObject->Listen())
		{
			CException l_uException("Listen Returned FALSE");
			throw l_uException;
		}
		{
			l_upCurrentObject->SetServerRunningStatus(true);
		}
		while(l_upCurrentObject->GetServerRunningStatus())
		{
			CFileHandler::GetInstance().WriteToFile(400,"CServerSocket::ThreadListenConnections:Waiting for New Connections Request..");
			int l_iNewConnectionFD = l_upCurrentObject->Accept(); 
			if(l_iNewConnectionFD >=0)
			{
				l_upCurrentObject->OnAccept(l_iNewConnectionFD);
			}
			else
			{
			//	perror("accept fail :");

				CFileHandler::GetInstance().WriteToSysLog("CServerSocket::ThreadListenConnections:Accept Failed..");
				CFileHandler::GetInstance().WriteToFile(400,"CServerSocket::ThreadListenConnections:Accept Failed");
				break;
			}
			usleep(1000);
		}
		CFileHandler::GetInstance().WriteToFile(400,"CServerSocket::ThreadListenConnections:Exiting from ThreadListenConnections..");
	}
	catch(CException a_uException)
	{
		l_upCurrentObject->SetServerRunningStatus(false);
		CFileHandler::GetInstance().WriteToFile(600,"Exception Raised in CServerSocket::ThreadListenConnections:%s",a_uException.what());
	}
	catch(...)
	{
		l_upCurrentObject->SetServerRunningStatus(false);
                CFileHandler::GetInstance().WriteToFile(600,"Unknown Exception Raised in CServerSocket::ThreadListenConnections:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
	}
	CFileHandler::GetInstance().WriteToFile(400,"Exiting from ThreadListenConnections");
	//pthread_exit(NULL);
	return NULL;
}

void* CServerSocket::ThreadMessageReceiver(void* a_upArguments)
{
	CServerSocket *l_upCurrentObject = (CServerSocket*)a_upArguments;
	try
	{
		int l_iSocketFD = l_upCurrentObject->GetSocketInstance()->GetSocketFD();
		while(l_upCurrentObject->GetSocketInstance()->GetConnectionStatus())
		{
			std::string l_sBuffer ;
			int l_iNumberOfBytes = -1;
//			CFileHandler::GetInstance().WriteToFile(400,"Waiting for data in ThreadMessageReceiver");
			if( l_upCurrentObject->ReceiveMessage(l_sBuffer,l_iNumberOfBytes))
			{
				if(l_iNumberOfBytes > 0)
				{
					l_upCurrentObject->OnMessageReceive(l_sBuffer,l_upCurrentObject->GetSocketInstance()->GetSocketFD());
				}	
			}
			else
			{
				l_upCurrentObject->GetSocketInstance()->SetConnectionStatus(false);
				l_upCurrentObject->OnDisconnect(l_upCurrentObject->GetSocketInstance()->GetSocketFD());
				break;
			}
			usleep(1000);

		}
		CFileHandler::GetInstance().WriteToFile(400,"Exiting from ThreadMessageReceiver for SocketFD:%d",l_iSocketFD);
	}	
	catch(CException a_uException)
	{
		CFileHandler::GetInstance().WriteToFile(600,"Exception Raised in CServerSocket::ThreadMessageReceiver:%s",a_uException.what());
	}
	catch(...)
	{
                CFileHandler::GetInstance().WriteToFile(600,"Unknown Exception Raised in CServerSocket::ThreadMessageReceiver:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
	}
	//pthread_exit(NULL);
	return NULL;
}
bool CServerSocket::StopServer()
{
	this->SetServerRunningStatus(false);
	sleep(1);
	this->UnSetSocketInstance();
	sleep(1);
}
bool CServerSocket::StartServer()
{
	// Make instance of socket with server instance status true
	try
	{
		this->m_upServerSocketObject = NULL;
		if ( !this->MakeServerSocketInstance(true) )
		{
			CException l_uException("MakeServerSocketInstance Returned FALSE");
			throw l_uException;
		}
	}
	catch(CException a_uException)
	{
		CFileHandler::GetInstance().WriteToFile(600,"Exception Raised in CServerSocket::StartServer:%s",a_uException.what());
		return false;
	}
	catch(...)
	{
		CFileHandler::GetInstance().WriteToFile(600,"Unknown Exception raised in CServerSocket::StartServer:Error in Making Server Socket Instance:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		return false;
	}
	// Create Thread for Listening Connections
	int l_iThreadAttrInitStatus = -1;
	pthread_attr_t l_uThreadAttribute;
	try
	{
		l_iThreadAttrInitStatus = pthread_attr_init(&l_uThreadAttribute);
		if(l_iThreadAttrInitStatus != 0 )
		{
			CException l_uException("Error in Initializing Thread Attributes for ThreadListenConnections");
			throw l_uException; 
		}
		int l_iSetDetachStateStatus = pthread_attr_setdetachstate(&l_uThreadAttribute, PTHREAD_CREATE_DETACHED);
		if(l_iSetDetachStateStatus != 0 )
		{
			CException l_uException("Error in Setting Detach state for ThreadListenConnections");
			throw l_uException; 
		}
		int l_iThreadCreationStatus = pthread_create(&CServerSocket::m_uListenThreadId, &l_uThreadAttribute,ThreadListenConnections , this);
		if(l_iThreadCreationStatus != 0 )
		{
			CException l_uException("Error in creating thread ThreadListenConnections");
			throw l_uException; 
		}
		CFileHandler::GetInstance().WriteToFile(400,"Started thread ThreadListenConnections");
		CFileHandler::GetInstance().WriteToSysLog("Started thread ThreadListenConnections");
		pthread_attr_destroy(&l_uThreadAttribute);
	}
	catch(CException a_uException)
	{
		if(l_iThreadAttrInitStatus == 0)
		{
			pthread_attr_destroy(&l_uThreadAttribute);
		}
		CFileHandler::GetInstance().WriteToFile(600,"Exception Raised in CServerSocket::StartServer:%s",a_uException.what());
		return false;
	}
	catch(...)
	{
		if(l_iThreadAttrInitStatus == 0)
		{
			pthread_attr_destroy(&l_uThreadAttribute);
		}
		CFileHandler::GetInstance().WriteToFile(600,"Unknown Exception raised in CServerSocket::StartServer:Error in creating ThreadListenConnections:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
		return false;
	}
}

