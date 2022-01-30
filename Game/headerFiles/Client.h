#ifndef CLIENT_H
#define CLIENT_H
#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <thread>
#include "headerFiles/ThreadedSocket.h"

class Client : public ThreadedSocket
{
protected:
	int id;
	char* buffer;

	void execute_thread();

	bool send_message(const char*);
	int recv_message();
	

public:
#ifdef _WIN32
	Client(int, SOCKET, const int MAXDATASIZE);
#else
	Client(int, int, const int MAXDATASIZE);
#endif
	~Client();
	void end_thread();
    int cardIndex(std::string);
    void removeCardFromHand(int);
    void addCardToHand(std::string);
    int getId();
    bool leaveGameOnDisconnect();
    std::vector<std::string> getCards();

private:
    int currentGameId;
    std::vector<std::string> cards;
};

#endif
