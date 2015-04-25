#include "CServerSocket.h"

int CServerSocket::m_isourcePortNumber = -1;
int CServerSocket::m_iserverConnectionQueueSize = 20;
std::string CServerSocket::m_ssourceIP = "";
SocketProtocol CServerSocket::m_uprotocol = InvalidSocketProtocol;
SocketType CServerSocket::m_usocketType = InvalidSocketType;
pthread_t CServerSocket::m_ulistenThreadId = -1;
bool CServerSocket::m_bserverRunningStatus = false;

CServerSocket::CServerSocket()
{
	m_upserverSocketObject = NULL;
}

CServerSocket::~CServerSocket()
{
	std::cout<<"Going to delete socket object"<<std::endl;
	if(m_upserverSocketObject!=NULL)
	{
		delete m_upserverSocketObject;
		m_upserverSocketObject = NULL;
		std::cout<<"Deleted socket object"<<std::endl;
	}
}
void CServerSocket::SetServerConnectionQueueSize(int a_iserverConnectionQueueSize)
{
	CServerSocket::m_iserverConnectionQueueSize = a_iserverConnectionQueueSize;
}

int CServerSocket::GetServerConnectionQueueSize()
{
	return CServerSocket::m_iserverConnectionQueueSize;
}

void CServerSocket::SetSourceIP(std::string a_ssourceIP)
{
	CServerSocket::m_ssourceIP = a_ssourceIP;
}
std::string CServerSocket::GetSourceIP()
{
	return CServerSocket::m_ssourceIP;
}
void CServerSocket::SetServerRunningStatus(bool a_bserverRunningStatus)
{
	CServerSocket::m_bserverRunningStatus = a_bserverRunningStatus;
}
bool CServerSocket::GetServerRunningStatus()
{
	return CServerSocket::m_bserverRunningStatus;
}
void CServerSocket::SetPortNumber(int a_iportNumber)
{
	CServerSocket::m_isourcePortNumber = a_iportNumber;
}

int CServerSocket::GetPortNumber()
{
	return CServerSocket::m_isourcePortNumber;
}

void CServerSocket::SetProtocol(SocketProtocol a_uprotocol)
{
	CServerSocket::m_uprotocol = a_uprotocol;
}

SocketProtocol CServerSocket::GetProtocol()
{
	return CServerSocket::m_uprotocol;
}

void CServerSocket::SetSocketType(SocketType a_usocketType)
{
	CServerSocket::m_usocketType = a_usocketType;
}

SocketType CServerSocket::GetSocketType()
{
	return CServerSocket::m_usocketType;
}

bool   CServerSocket::MakeServerSocketInstance(bool a_bnewSocketFlag)
{
	return this->MakeServerSocketInstance(a_bnewSocketFlag,0);
}
bool   CServerSocket::MakeServerSocketInstance(bool a_bnewSocketFlag,int a_isocketFD)
{
	CBasicSocket* l_upsocketObject = NULL;

	CSocketFactory *l_upsocketFactory = NULL;
	//Create the socket Factory
	try
	{
		switch(CServerSocket::GetSocketType())
		{
			case SynchronousSocket:
				l_upsocketFactory = new CSynchronousSocketFactory;
				if( l_upsocketFactory == NULL)
					throw;
				break;
			default:
				break;
		};
	} catch(...)
	{
		std::cout<<"Error in creating Socket Factory"<<std::endl;
		return false;
	}	
	// Create the Socket Object
	try
	{
		l_upsocketObject = l_upsocketFactory->MakeSocketObject(CServerSocket::m_uprotocol);	
		if( l_upsocketObject == NULL)
			throw;
		delete l_upsocketFactory;
	} catch(...)
	{
		if(l_upsocketFactory != NULL)
		{
			delete l_upsocketFactory;
		}
		std::cout<<"Error in Creating Server Socket Instance"<<std::endl;
		return false;
	}
	this->SetSocketInstance(l_upsocketObject);
	if(a_bnewSocketFlag)
	{
		if (! this->GetSocketInstance()->Create() )
		{
			return false;
		}
		this->SetServerRunningStatus(true);
	}
	else
	{
		this->GetSocketInstance()->SetSocketFD(a_isocketFD);
	}
	this->GetSocketInstance()->SetConnectionStatus(true);
	return true;
}

CBasicSocket* CServerSocket::GetSocketInstance()
{
	return this->m_upserverSocketObject;
}

void CServerSocket::SetSocketInstance(CBasicSocket* a_upsocketObject)
{
	this->m_upserverSocketObject = a_upsocketObject;
}
bool CServerSocket::SendMessage(std::string a_sdata, int& a_imessageLength)
{
	return this->GetSocketInstance()->Send(a_sdata, a_imessageLength);
}

bool CServerSocket::ReceiveMessage(std::string& a_sdata, int& a_ireceivedDataLength)
{
	return this->GetSocketInstance()->Receive(a_sdata,a_ireceivedDataLength);
}

void CServerSocket::ForceDisconnect(int a_isocketFD)
{
	CBasicSocket::ForceClose(a_isocketFD);
	CBasicSocket::ForceShutdown(a_isocketFD);
}
bool CServerSocket::DisconnectClient()
{
	return this->GetSocketInstance()->Close();
	return this->GetSocketInstance()->Shutdown();
}

void CServerSocket::OnAccept(int a_isocketFD)
{
}

void CServerSocket::OnMessageReceive(std::string a_sreceivedMessage, int a_ireceivedMessageLength)
{
}

void CServerSocket::OnDisconnect(int a_isocketFD)
{
}
bool CServerSocket::Bind()
{
	if( this->GetSocketInstance()->Bind(CServerSocket::m_ssourceIP,CServerSocket::m_isourcePortNumber) )
	{
		return true;
	}
	else
	{
		return false;
	}

}
int CServerSocket::Accept()
{
	return this->GetSocketInstance()->Accept();
}
bool CServerSocket::Listen()
{
	return this->GetSocketInstance()->Listen(CServerSocket::m_iserverConnectionQueueSize);
}

void* CServerSocket::ThreadListenConnections(void* a_uparguments)
{
	CServerSocket *l_upcurrentObject = (CServerSocket*)a_uparguments;
	//Bind to source IP and Port
	if(!l_upcurrentObject->Bind())
	{
		l_upcurrentObject->SetServerRunningStatus(false);
		pthread_exit(NULL);	
	}

	//Listen to the socket
	if(!l_upcurrentObject->Listen())
        {
		l_upcurrentObject->SetServerRunningStatus(false);
                pthread_exit(NULL);
        }
	{
		l_upcurrentObject->SetServerRunningStatus(true);
	}
	while(l_upcurrentObject->GetServerRunningStatus())
	{
		int l_inewConnectionFD = l_upcurrentObject->Accept(); 
		if(l_inewConnectionFD >=0)
		{
			l_upcurrentObject->OnAccept(l_inewConnectionFD);
		}
		else
		{
			perror("accept fail :");
			sleep(1);
			
		}
		usleep(1000);
	}
	std::cout<<"Exiting from ThreadListenConnections.."<<std::endl;
	pthread_exit(NULL);
	return NULL;
}

void* CServerSocket::ThreadMessageReceiver(void* a_uparguments)
{
	CServerSocket *l_upcurrentObject = (CServerSocket*)a_uparguments;

	while(l_upcurrentObject->GetSocketInstance()->GetConnectionStatus())
	{
		std::string l_sbuffer ;
		int l_inumberOfBytes = -1;
		std::cout<<"Waiting for data in ThreadMessageReceiver"<<std::endl;
		if( l_upcurrentObject->ReceiveMessage(l_sbuffer,l_inumberOfBytes))
		{
			std::cout<<"Received Data Length = "<<l_inumberOfBytes<<std::endl;
			if(l_inumberOfBytes > 0)
			{
				std::cout<<"Caling OnMessageReceive.."<<std::endl;
				l_upcurrentObject->OnMessageReceive(l_sbuffer,l_upcurrentObject->GetSocketInstance()->GetSocketFD());
			}	
		}
		else
		{
			l_upcurrentObject->GetSocketInstance()->SetConnectionStatus(false);
			l_upcurrentObject->OnDisconnect(l_upcurrentObject->GetSocketInstance()->GetSocketFD());
			break;
		}
		usleep(1000);
	}
	std::cout<<"Exiting from ThreadMessageReceiver for SocketFD:"<<l_upcurrentObject->GetSocketInstance()->GetSocketFD()<<std::endl;
	pthread_exit(NULL);
	return NULL;
}

bool CServerSocket::StartServer()
{
	// Make instance of socket with server instance status true
	try
	{
		this->m_upserverSocketObject = NULL;
		if ( !this->MakeServerSocketInstance(true) )
		{
			throw;
		}
	}
	catch(...)
	{
		std::cout<<"Error in Making Server Socket Instance"<<std::endl;
		return false;
	}
	// Create Thread for Listening Connections
	try
	{
		pthread_attr_t l_uthreadAttribute;
		int l_ithreadAttrInitStatus = pthread_attr_init(&l_uthreadAttribute);
		if(l_ithreadAttrInitStatus != 0 )
		{
			std::cout<<"Error in Initializing Thread Attributes for ThreadListenConnections"<<std::endl;
			throw;
		}
		int l_isetDetachStateStatus = pthread_attr_setdetachstate(&l_uthreadAttribute, PTHREAD_CREATE_DETACHED);
		if(l_isetDetachStateStatus != 0 )
		{
			std::cout<<"Error in Setting Detach state for ThreadListenConnections"<<std::endl;
			throw;
		}
		int l_ithreadCreationStatus = pthread_create(&CServerSocket::m_ulistenThreadId, &l_uthreadAttribute,ThreadListenConnections , this);
		if(l_ithreadCreationStatus != 0 )
		{
			std::cout<<"Error in creating thread ThreadListenConnections"<<std::endl;
			throw;
		}
		pthread_attr_destroy(&l_uthreadAttribute);
	}
	catch(...)
	{
		std::cout<<"Error in creating ThreadListenConnections"<<std::endl;
		return false;
	}
}

