#ifndef __CSOCKETFACTORY_H__
#define __CSOCKETFACTORY_H__
#include "CBasicSocket.h"

class CSocketFactory
{
private:
public:
	virtual CBasicSocket* MakeSocketObject(int a_iprotocol) = 0;
};
#endif
