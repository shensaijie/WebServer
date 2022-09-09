#pragma once
#include "Common.h"
#include "Socket.h"

#include <memory>

class InetAddress;

class TcpStream;
typedef std::unique_ptr<TcpStream> TcpStreamPtr;

class Acceptor : noncopyable {
public:
    Acceptor(const InetAddress& listenAddr);
    ~Acceptor() = default;
    Acceptor(Acceptor&&) = default;
    Acceptor& operator=(Acceptor&&) = default;

    //thread safe 线程安全
    TcpStreamPtr accept();
    Socket acceptSocket();

private:
    Socket listenSock_;
};
