#ifndef __CSYNCHRONOUSSOCKETFACTORY_H__
#define __CSYNCHRONOUSSOCKETFACTORY_H__
#include "CSocketFactory.h"
#include "CTCPSocket.h"

class CSynchronousSocketFactory:public CSocketFactory
{
public:
	CSynchronousSocketFactory();
	~CSynchronousSocketFactory();
	CBasicSocket* MakeSocketObject(int);
};
#endif
