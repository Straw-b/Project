
SVR_BIN=../bin/ChatServer
CLI_BIN=../bin/ChatClient
INLCUDE_PATH=-I /usr/include/jsoncpp

all:$(SVR_BIN) $(CLI_BIN)
$(SVR_BIN):ChatServer.cpp
	g++ $^ -o $@ -g -lpthread -std=c++11 -ljsoncpp $(INLCUDE_PATH) 

$(CLI_BIN):ChatClient.cpp
	g++ $^ -o $@ -g -lpthread -std=c++11 -ljsoncpp $(INLCUDE_PATH) -lncurses

.PHONY:clean
clean:
	rm $(SVR_BIN) $(CLI_BIN) 
