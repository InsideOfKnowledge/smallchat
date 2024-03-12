#include "chatServer.h"
#include <iostream>

int main()
{
    if (!ChatServer::getInstance().start())
        return -1;

    return 0;
}