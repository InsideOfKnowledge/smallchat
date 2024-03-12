#include "chatServer.h"
#include <iostream>

int main()
{
    ChatServer chatServer;

    chatServer.init();

    chatServer.start();

    return 0;
}