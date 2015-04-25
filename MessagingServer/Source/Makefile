CC= g++ -w -g
OBJS = CBasicSocket.o CMessagingServer.o CServerSocket.o CSynchronousSocketFactory.o CTCPSocket.o MessagingServerMain.o 

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
	$(CC) $(MYSQL) -c MessagingServerMain.cpp
	
clean:
	rm -f *.o
	rm -f MessagingServer

