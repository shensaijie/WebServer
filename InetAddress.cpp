#include "InetAddress.h"
#include <memory>
#include <cassert>
#include <iostream>

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



