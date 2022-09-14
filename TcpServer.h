#pragma once
#include "TcpConnection.h"
#include <memory>

typedef std::unique_ptr<TcpConnection> TcpConnectionPtr;
class TcpServer {
public:
    void startUp();
    void* requestHelper(void* args);
};
