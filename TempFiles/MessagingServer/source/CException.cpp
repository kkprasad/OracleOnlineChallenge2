#include "CException.h"
/*******************************************************************************
File Name	        : CException.cpp
Class Name		: CException
Description    		: This file contains the methods for exception handling
*******************************************************************************/
using namespace std;

CException::CException() {
	this->m_sMessage = "";
}

CException::CException( const char* pMsg) {
	this->m_sMessage = pMsg;
}

CException::CException( std::string sMsg) {
	this->m_sMessage = sMsg;
}

CException::~CException() throw() {
	this->m_sMessage = "";
}

const char* CException::what() const throw() {
	return this->m_sMessage.c_str();
}


