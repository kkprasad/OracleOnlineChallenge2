CC= g++ -w -g
OBJS = CBasicSocket.o CMessagingServer.o CServerSocket.o CSynchronousSocketFactory.o CTCPSocket.o MessagingServerMain.o CConfigData.o CFileReader.o

All: MessagingServer
	
default: All

MessagingServer: $(OBJS)
	$(CC) $(OBJS)  -lpthread -o MessagingServer

CBasicSocket.o:
	$(CC) -c CBasicSocket.cpp
CMessagingServer.o:
	$(CC) -c CMessagingServer.cpp
CServerSocket.o:
	$(CC) -c CServerSocket.cpp
CSynchronousSocketFactory.o:
	$(CC) -c CSynchronousSocketFactory.cpp
CTCPSocket.o:
	$(CC) -c CTCPSocket.cpp
MessagingServerMain.o:
	$(CC) -c MessagingServerMain.cpp
CConfigData.o:
	$(CC) -c CConfigData.cpp
CFileReader.o:
	$(CC) -c CFileReader.cpp

	
clean:
	rm -f *.o
	rm -f MessagingServer

