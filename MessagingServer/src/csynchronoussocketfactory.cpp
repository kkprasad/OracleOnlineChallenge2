/*******************************************************************************
File Name               : csynchronoussocketfactory.cpp
Class Name              : CSynchronousSocketFactory
Description             : This file contains the factory methods to create the 
			  socket object
*******************************************************************************/

#include <csynchronoussocketfactory.h>

CBasicSocket* CSynchronousSocketFactory::MakeSocketObject(int a_iprotocol)
{
/*********************************************************************
Function Name		: MakeSocketObject
Arguments Description 	: Socket Protocol
Return Value		: Socket Object corresponding to Protocol
Function Description	: This method is to make the Socket object
			  as an objective of the Factory
*********************************************************************/
	CBasicSocket *l_upbaskcSocket = NULL;
	try
	{
		switch (a_iprotocol)
		{
			case IPPROTO_TCP:
				l_upbaskcSocket = new CTCPSocket;
				break;
			default:
				break;
				//return NULL;
		};

		if(l_upbaskcSocket == NULL )
		{
			CException l_uexception("Error in allocating memory for Socket in CSynchronousSocketFactory::MakeSocketObject");;
			throw l_uexception;
		}
		else
		{
			return l_upbaskcSocket;
		}
	}
	catch(CException a_uException)
	{
		if (l_upbaskcSocket != NULL)
                {
                        delete l_upbaskcSocket;
                        l_upbaskcSocket = NULL;
                }
		CFileHandler::GetInstance().WriteToFile(700,"Unknown Exception Raised in CSynchronousSocketFactory::MakeSocketObject::%s",a_uException.what());
		throw a_uException;
	}
	catch (...)
	{
		if (l_upbaskcSocket != NULL)
		{
			delete l_upbaskcSocket;
			l_upbaskcSocket = NULL;
		}
		char l_cpDescription[1024] = {0x00};
                sprintf(l_cpDescription,"Unknown Exception Raised in CSynchronousSocketFactory::MakeSocketObject:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
                CException l_uException(l_cpDescription);
		CFileHandler::GetInstance().WriteToFile(700,"Unknown Exception Raised in CSynchronousSocketFactory::MakeSocketObject:ErrorNumber:%d,ErrorString:%s",errno,strerror(errno));
                throw l_uException;
	
	}
}
CSynchronousSocketFactory::CSynchronousSocketFactory()
{
}
CSynchronousSocketFactory::~CSynchronousSocketFactory()
{
}

