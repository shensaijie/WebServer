#include "TcpStream.h"
#include "Channel.h"

#include <unistd.h>
#include <string>
#include <iostream>
#include <sstream>
#include <cstring>

TcpStream::TcpStream(Socket&& sock) : sock_(std::move(sock)) { }

void* TcpStream::request() {
    std::string buf, word;
    while(receiveLine(buf)==0);
    std::istringstream in(buf);
    in >> word;
    if(::strcasecmp(word.c_str(), "GET") && ::strcasecmp(word.c_str(), "POST")) {
	//请求方式错误
	return nullptr;
    }
    
    if(::strcasecmp(word.c_str(), "POST") == 0)
	post_ = cgi_ = true;
    else 
	post_ = false;
    
    in >> word;

    //GET请求有可能带？
    if(!post_) {
	if(word.find('?') != std::string::npos)
	    cgi_ = true;
    }
    path_ = "httpdocs" + word;

    while(receiveLine(buf) && buf!="\n") buf = "";

    Channel chan(path_);
    std::string &temp = chan.path();
    std::string &mes = chan.run();
    sendAll(mes.c_str(), mes.size());
    return nullptr;
}

int TcpStream::receiveAll(void* buf, int len) {
    return ::recv(sock_.fd(), buf, len, MSG_WAITALL);
}

int TcpStream::receiveSome(void* buf, int len) {
    return sock_.recv(buf, len);
}

int TcpStream::receiveLine(std::string &str) {
    char c = '\0';
    while(c != '\n') {
	if(::recv(sock_.fd(), &c, 1, 0) > 0) {
	    if(c == '\r') {
		if(::recv(sock_.fd(), &c, 1, MSG_PEEK) > 0 && c=='\n')
		    ::recv(sock_.fd(), &c, 1, 0);
		else
		    c = '\n';
	    }
	    str += c;
	}
	else 
	    c = '\n';
    }
    return str.size();
}


int TcpStream::sendAll(const void* buf, int len) {
    int written = 0;
    while(written < len) {
	int nw = sock_.send(static_cast<const char*>(buf) + written, len - written);
	if(nw > 0) {
	    written += nw;
	} else {
	    break;
	}
    }
    return written; //返回写入的数量
}

int TcpStream::sendSome(const void* buf, int len) {
    return sock_.send(buf, len);
}
