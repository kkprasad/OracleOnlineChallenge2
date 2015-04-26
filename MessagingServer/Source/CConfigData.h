#ifndef __CCONFIGDATA_H__
#define __CCONFIGDATA_H__
#include<iostream>
#include "Definitions.h"
#include "CFileReader.h"

class CConfigData
{
private:
	std::string	m_sconfigurationFileName;
	std::string	m_sserverSourceIP;
	int		m_iserverPortNumber;
	int		m_imaxMessageHandlerThreads;
	int		m_imaxMessageSenderThreads;
	SocketProtocol	m_uprotocol;
	SocketType	m_usocketType;
	CFileReader	m_ufileReader;
public:
	CConfigData();
	~CConfigData();
	bool LoadConfigurationData();
	std::string GetConfigurationFileName();
	SocketProtocol GetProtocolType();
	SocketType GetSocketType();
	int GetServerPortNumber();
	std::string GetServerSourceIP();

	void SetConfigurationFileName(std::string);
	void SetServerSourceIP(std::string);
	void SetServerPortNumber(int);
	void SetProtocol(std::string);
	void SetSocketType(std::string);	
	int GetMaxMessageHandlerThreads();
	void SetMaxMessageHandlerThreads(int);	
	int GetMaxMessageSenderThreads();
	void SetMaxMessageSenderThreads(int);
	
	std::string  GetParam( std::string );
};
#endif

