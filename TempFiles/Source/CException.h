#ifndef _CEXCEPTION_H__
#define _CEXCEPTION_H__

#include <exception>
#include <string>

class CException:public std::exception
{
public:
	CException();
	CException( const char*);
	CException( std::string);
	virtual ~CException() throw();
	virtual const char* what() const throw();
private:
	std::string		m_sMessage;
};
#endif


