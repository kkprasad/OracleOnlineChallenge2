CC= g++ -w -g
INCLUDE = -I.
OBJS = CBasicSocket.o CMessagingServer.o CServerSocket.o CSynchronousSocketFactory.o CTCPSocket.o MessagingServerMain.o CConfigData.o CFileHandler.o CException.o

All: msgserverd
	
default: All

msgserverd: $(OBJS)
	$(CC) $(OBJS)  -lpthread -o msgserverd
	rm -f *.o
CBasicSocket.o:
	$(CC) $(INCLUDE) -c CBasicSocket.cpp
CMessagingServer.o:
	$(CC) $(INCLUDE) -c CMessagingServer.cpp
CServerSocket.o:
	$(CC) $(INCLUDE) -c CServerSocket.cpp
CSynchronousSocketFactory.o:
	$(CC) $(INCLUDE) -c CSynchronousSocketFactory.cpp
CTCPSocket.o:
	$(CC) $(INCLUDE) -c CTCPSocket.cpp
MessagingServerMain.o:
	$(CC) $(INCLUDE) -c MessagingServerMain.cpp
CConfigData.o:
	$(CC) $(INCLUDE) -c CConfigData.cpp
CFileHandler.o:
	$(CC) $(INCLUDE) -c CFileHandler.cpp
CException.o:
	$(CC) $(INCLUDE) -c CException.cpp

	
clean:
	rm -f *.o
	rm -f msgserverd

