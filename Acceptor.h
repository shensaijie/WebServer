#pragma once
#include "Common.h"
#include "Socket.h"

#include <memory>

class InetAddress;

class TcpConnection;
typedef std::unique_ptr<TcpConnection> TcpConnectionPtr;

class Acceptor : noncopyable {
public:
    Acceptor(const InetAddress& listenAddr);
    ~Acceptor() = default;
    Acceptor(Acceptor&&) = default;
    Acceptor& operator=(Acceptor&&) = default;

    //thread safe 线程安全
    TcpConnectionPtr accept();
    Socket acceptSocket();

private:
    Socket listenSock_;
};
