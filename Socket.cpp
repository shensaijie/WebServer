#include "Socket.h"
#include "InetAddress.h"

#include <cassert>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>

Socket::Socket(int sockfd) : sockfd_(sockfd) {
   assert(sockfd_ >= 0);
}

Socket::~Socket(){
    if(sockfd_ >= 0){
	int ret = ::close(sockfd_);
	assert(ret == 0);
	(void)ret;
    }
}

void Socket::bind(const InetAddress& addr) {
    int ret = ::bind(sockfd_, addr.get_sockaddr(), addr.lenght());
    if(ret){
	perror("Socket::bind");   //输出函数的错误
	abort();                       //立即终止程序，不释放资源
    }
}

void Socket::listen() {
    int ret = ::listen(sockfd_, SOMAXCONN);
    if(ret){
	perror("Socket::listen");
	abort();
    }
}

void Socket::setReuseAddr(bool on) {
    int optval = on ? 1 : 0;
    if( ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, 
		    &optval, sizeof optval) < 0 ) {
	perror("Socket::setReuseAddr");
    }
}

int Socket::recv(void* buf, int len) {
    return ::recv(sockfd_, buf, len, 0);
}

int Socket::send(const void* buf, int len) {
    return ::send(sockfd_, buf, len, 0);
}

Socket Socket::createTCP(sa_family_t family) {
    int sockfd = ::socket(family, SOCK_STREAM, 0);
    assert(sockfd >= 0);
    return Socket(sockfd);
}

Socket Socket::createUDP(sa_family_t family) {
    int sockfd = ::socket(family, SOCK_STREAM, 0);
    assert(sockfd >= 0);
    return Socket(sockfd);
}
