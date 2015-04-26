#include "CConfigData.h"

CConfigData::CConfigData()
{
	this->m_sconfigurationFileName = "";
	this->m_uprotocol = InvalidSocketProtocol;
	this->m_usocketType = InvalidSocketType;
	this->m_iserverPortNumber = 0;
	this->m_sserverSourceIP = "";
	this->m_imaxMessageHandlerThreads = 0;
	this->m_imaxMessageSenderThreads = 0;
}
CConfigData::~CConfigData()
{
}

std::string CConfigData::GetConfigurationFileName()
{
	return this->m_sconfigurationFileName;
}

void CConfigData::SetConfigurationFileName(std::string a_sconfigurationFileName)
{
	this->m_sconfigurationFileName = a_sconfigurationFileName;
}

int CConfigData::GetMaxMessageHandlerThreads()
{
	return this->m_imaxMessageHandlerThreads;
}
void CConfigData::SetMaxMessageHandlerThreads(int a_imaxMessageHandlerThreads)
{
	this->m_imaxMessageHandlerThreads = a_imaxMessageHandlerThreads;
}
int CConfigData::GetMaxMessageSenderThreads()
{
	return this->m_imaxMessageSenderThreads;
}
void CConfigData::SetMaxMessageSenderThreads(int a_imaxMessageSenderThreads)
{
	this->m_imaxMessageSenderThreads = a_imaxMessageSenderThreads;
}

std::string CConfigData::GetParam( std::string a_sparameter)
{
	std::string l_sparamDefaultVal = "";
	std::string l_ssection = "MessagingServer";
	std::string l_skey = a_sparameter;
	char l_cpparamVal[256];
	if ( this->m_ufileReader.GetPrivateProfileString( l_ssection.c_str(), l_skey.c_str(), l_sparamDefaultVal.c_str(), l_cpparamVal, this->m_sconfigurationFileName.c_str() ) == -1 ) {
		return "";
	}

	l_sparamDefaultVal = "";

	l_sparamDefaultVal.append( l_cpparamVal);

	return l_sparamDefaultVal;
}

bool CConfigData::LoadConfigurationData()
{
	int l_itemp = 0;
	std::string l_stemp;
	//m_uprotocol
	this->SetProtocol(this->GetParam("Protocol"));
	//m_usocketType
	this->SetSocketType(this->GetParam("SocketType"));
	//m_iserverPortNumber

	l_stemp = this->GetParam("PortNumber");
	l_itemp = atoi(l_stemp.c_str());

	this->SetServerPortNumber(l_itemp);
	//m_sserverSourceIP
	this->SetServerSourceIP(this->GetParam("SourceServerIP"));
	l_stemp = "";	
	l_stemp = this->GetParam("MaxMessageHandlerThreads");
	l_itemp = atoi(l_stemp.c_str());
	this->SetMaxMessageHandlerThreads(l_itemp);

	l_stemp = this->GetParam("MaxMessageSendThreads");
        l_itemp = atoi(l_stemp.c_str());
	this->SetMaxMessageSenderThreads(l_itemp);
	
	return true;
}

SocketProtocol CConfigData::GetProtocolType()
{
	return this->m_uprotocol;
}

SocketType CConfigData::GetSocketType()
{
	return this->m_usocketType;
}

int CConfigData::GetServerPortNumber()
{
	return this->m_iserverPortNumber;
}

std::string CConfigData::GetServerSourceIP()
{
	return this->m_sserverSourceIP;
}

void CConfigData::SetServerSourceIP(std::string a_sserverSourceIP)
{
	this->m_sserverSourceIP = a_sserverSourceIP;
}

void CConfigData::SetServerPortNumber(int a_iserverPortNumber)
{
	this->m_iserverPortNumber = a_iserverPortNumber;
}

void CConfigData::SetProtocol(std::string a_sprotocol)
{
	if(a_sprotocol == "TCPSocket")
	{
		this->m_uprotocol = TCPSocket;
	}
	else if(a_sprotocol == "UDPSocket")
	{
		this->m_uprotocol = SCTPSocket;
	}
	else if(a_sprotocol == "SCTPSocket")
	{
		this->m_uprotocol = SCTPSocket;
	}
	else
	{
		this->m_uprotocol = InvalidSocketProtocol;
	}
}

void CConfigData::SetSocketType(std::string a_ssocketType)
{
	if(a_ssocketType == "SynchronousSocket")
	{
		this->m_usocketType = SynchronousSocket;
	}
	else if(a_ssocketType == "AsynchronousSocket")
	{
		this->m_usocketType = AsynchronousSocket;
	}
	else
	{
		this->m_usocketType = InvalidSocketType;
	}
}


