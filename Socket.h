#pragma once
#include "Common.h"

#include <utility>
#include <netinet/in.h>

class InetAddress;

class Socket : noncopyable {
public:
    Socket(int sockfd);
    ~Socket();

    Socket(Socket&& rhs) : Socket(rhs.sockfd_) {//&&是右值引用，是数据,不一定可寻址
	rhs.sockfd_ = -1;
    }
   
    Socket& operator=(Socket&& rhs) {
	swap(rhs);
	return *this;
    }

    void swap(Socket& rhs) {
	std::swap(sockfd_, rhs.sockfd_);
    }
    
    int fd() {
	return sockfd_;
    }

    //Socket API
    void bind(const InetAddress& addr);
    void listen();

    //成功则返回0
    void setReuseAddr(bool on);
    void setTcpNoDelay(bool on);

    int recv(void* buf, int len);
    int send(const void* buf, int len);

    //factory methods 工厂模式
    static Socket createTCP(sa_family_t family);
    static Socket createUDP(sa_family_t family);

private:
    int sockfd_;
};
