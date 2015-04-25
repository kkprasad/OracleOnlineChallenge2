#include "CTCPSocket.h"

bool CTCPSocket::Create()
{
	int l_isocketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(l_isocketFD==-1) 
		return false;
	this->SetSocketFD( l_isocketFD );
	this->SetSocketCreatedStatus(true);
	return true;
}

bool CTCPSocket::Bind(std::string a_ssourceIP,int a_isourcePortNumber)
{
	int l_isocketOption = 1; 
	if (setsockopt(this->GetSocketFD(),SOL_SOCKET,SO_REUSEADDR,(char *)&l_isocketOption,sizeof(l_isocketOption)) == -1)
	{
		perror("setsockopt");
		return false;
	}
	struct sockaddr_in l_usockAddr;

	l_usockAddr.sin_port=htons(a_isourcePortNumber);
	l_usockAddr.sin_addr.s_addr=htonl(INADDR_ANY);

	int l_bindRetryCount=0;
	while(bind(this->GetSocketFD(),(sockaddr *)&(l_usockAddr),sizeof(l_usockAddr)) == -1)
	{
		if(l_bindRetryCount++ >10)
		{
			printf("\nStop Binding Retry .... \n");
			return false;
		}
		fprintf(stdout," Bind Fail Retrying ...%d\n",l_bindRetryCount);
		perror("");
		sleep(1);
	}
	printf("Bind Success ... 0n m_iSocketFD = %d\n",this->GetSocketFD()); 
	return true;
}
bool CTCPSocket::Send(std::string a_sdata , int& a_imessageLength)
{
	a_imessageLength = send(this->GetSocketFD(), a_sdata.c_str(), a_sdata.length(), MSG_NOSIGNAL);
	return true;	
}

bool CTCPSocket::Receive(std::string& a_sreceivedData, int& a_ireceivedMessageLength)
{
	char l_cpbuffer[MAX_BUFFER_SIZE]={0x00};
	bool l_breceiveStatus = false;

	a_ireceivedMessageLength = recv(this->GetSocketFD(), l_cpbuffer, MAX_BUFFER_SIZE - 1,MSG_NOSIGNAL);
	std::cout<<"Received Buffer is..."<<std::endl;
	for(int i=0;i<a_ireceivedMessageLength;i++)
	{
		fprintf(stdout,"%02X ",l_cpbuffer[i]);
	}
	if(a_ireceivedMessageLength == 0)
	{
		l_breceiveStatus = false;
	}
	else if(a_ireceivedMessageLength == -1)
	{
		a_sreceivedData = "";
		l_breceiveStatus = false;
	}
	else if(a_ireceivedMessageLength >0)
	{	
		a_sreceivedData = l_cpbuffer;
		l_breceiveStatus = true;
	}
	return l_breceiveStatus;
}
bool CTCPSocket::Connect(std::string a_sdestinationIP, int a_idestinationPortNumber)
{
	struct sockaddr_in l_usockAddr;
	l_usockAddr.sin_port=htons(a_idestinationPortNumber);
	l_usockAddr.sin_addr.s_addr=inet_addr(a_sdestinationIP.c_str());
	int l_iconnectStatus = connect(this->GetSocketFD(), (struct sockaddr*)&l_usockAddr, sizeof(l_usockAddr));
	if(l_iconnectStatus == -1) 
	{
		this->SetConnectionStatus(false);
		return false;
	}
	else
	{
		this->SetConnectionStatus(true);
		return true;
	}	
}



