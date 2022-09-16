#include "InetAddress.h"

#include <memory>
#include <assert.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>

using namespace std;

InetAddress::InetAddress(const std::string ip, uint16_t port) {
    setPort(port);

    int result = 0;
    if(ip.find(":")==std::string::npos){
	result = ::inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr);
	addr_.sin_family = AF_INET;
    } else {
	result = ::inet_pton(AF_INET6, ip.c_str(), &addr6_.sin6_addr);
	addr6_.sin6_family = AF_INET6;
    }
    assert(result == 1 && "Invalid IP format");
}

InetAddress::InetAddress(uint16_t port) {
    memset(&addr_, 0, sizeof addr_);
    addr_.sin_family = AF_INET;
    in_addr_t ip = INADDR_ANY;
    addr_.sin_addr.s_addr = htonl(ip);
    addr_.sin_port = htons(port);
}

