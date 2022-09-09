#pragma once
#include "Common.h"
#include "InetAddress.h"
#include "Socket.h"

#include <memory>
//typedef std::unique_ptr<TcpStream> TcpStreamPtr;

class TcpStream : noncopyable {
public:
    TcpStream(Socket&& sock);
    ~TcpStream() = default;

    //处理监听到的HTTP请求
    void *request();

    //recv send 发送接受数据
    int receiveAll(void* buf, int len);
    int receiveSome(void* buf, int len); 
    int receiveLine(std::string&);

    int sendAll(const void* buf, int len);
    int sendSome(const void* buf, int len);
    
    int fd() { return sock_.fd(); }
private:
    Socket sock_;  
    bool post_;
    bool cgi_;
    std::string path_;
};
