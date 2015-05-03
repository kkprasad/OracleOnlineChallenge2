#ifndef __CTCPSOCKET_H__
#define __CTCPSOCKET_H__
#include <cbasicsocket.h>
#include <cexception.h>
#include <cfilehandler.h>

class CTCPSocket:public CBasicSocket
{
private:
public:
	CTCPSocket();
	~CTCPSocket();
	bool Create();
	bool Bind(std::string,int);
	bool Send(std::string ,int&);
	bool Receive(std::string&,int&);
	bool Connect(std::string,int);

};
#endif
