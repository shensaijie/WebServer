#pragma once    //保证只编译一次

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>

class InetAddress {
public:
    InetAddress() { addr_.sin_family = AF_UNSPEC; }
    InetAddress(const std::string ip,uint16_t port);

    void setPort(uint16_t port) { addr_.sin_port = htons(port); }
    uint16_t port() const { return ntohs(addr_.sin_port);}
    sa_family_t family() const { return addr_.sin_family; }

    //套接字接口
    const struct sockaddr* get_sockaddr() const{
	return (const struct sockaddr*)(&addr_);
    }

    socklen_t lenght() const{
	return family() == AF_INET6 ? sizeof addr6_ : sizeof addr_;
    }


private:
    union{
	struct sockaddr_in addr_;
	struct sockaddr_in6 addr6_;
    };
};
