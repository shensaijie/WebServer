#pragma once
#include "TcpConnection.h"
#include <memory>

typedef std::unique_ptr<TcpConnection> TcpConnectionPtr;
class TcpServer {
public:
    void startUp();
private:
    static void* requestHelper(void* args);
    TcpConnection* tcp;
};
