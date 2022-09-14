#include "Acceptor.h"
#include "InetAddress.h"
#include "TcpConnection.h"

#include <cstdio>
#include <sys/socket.h>

Acceptor::Acceptor(const InetAddress& listenAddr) 
    : listenSock_(Socket::createTCP(listenAddr.family())) {
    
    listenSock_.setReuseAddr(true);
    listenSock_.bind(listenAddr);
    listenSock_.listen();
}

TcpConnectionPtr Acceptor::accept() {
    int sockfd = ::accept(listenSock_.fd(), nullptr, nullptr);
    if (sockfd >= 0) {
	return TcpConnectionPtr(new TcpConnection(Socket(sockfd)));
    } else {
	perror("Acceptor::accept: ");
	return TcpConnectionPtr();
    }
}

Socket Acceptor::acceptSocket() {
    int sockfd = ::accept(listenSock_.fd(), nullptr, nullptr);
    if (sockfd >= 0) {
	return Socket(sockfd);
    } else {
	perror("Acceptor::acceptSocket: ");
	abort();
    }
}
