CC= g++ -w -g
INCLUDE = -I.
OBJS = cbasicsocket.o cmessagingserver.o cserversocket.o csynchronoussocketfactory.o ctcpsocket.o messagingservermain.o cconfigdata.o cfilehandler.o cexception.o

All: msgserverd
	
default: All

msgserverd: $(OBJS)
	$(CC) $(OBJS)  -lpthread -o msgserverd
	rm -f *.o
cbasicsocket.o:
	$(CC) $(INCLUDE) -c cbasicsocket.cpp
cmessagingserver.o:
	$(CC) $(INCLUDE) -c cmessagingserver.cpp
cserversocket.o:
	$(CC) $(INCLUDE) -c cserversocket.cpp
csynchronoussocketfactory.o:
	$(CC) $(INCLUDE) -c csynchronoussocketfactory.cpp
ctcpsocket.o:
	$(CC) $(INCLUDE) -c ctcpsocket.cpp
messagingservermain.o:
	$(CC) $(INCLUDE) -c messagingservermain.cpp
cconfigdata.o:
	$(CC) $(INCLUDE) -c cconfigdata.cpp
cfilehandler.o:
	$(CC) $(INCLUDE) -c cfilehandler.cpp
cexception.o:
	$(CC) $(INCLUDE) -c cexception.cpp

	
clean:
	rm -f *.o
	rm -f msgserverd

