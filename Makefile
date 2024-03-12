all: mysmallchat-server
# CFLAGS=-O2 -Wall -W -std=c99
CFLAGS=-g -O0 -Wall -W -std=c++11
cc=g++

mysmallchat-server: chatServer.cpp main.cpp
	$(cc) chatServer.cpp main.cpp -o mysmallchat-server $(CFLAGS)


clean:
	rm -f mysmallchat-server