#pragma once
#include "Common.h"
#include "InetAddress.h"
#include "Socket.h"

#include <memory>

class TcpConnection;
typedef std::unique_ptr<TcpConnection> TcpConnectionPtr;

class TcpConnection {
public:
    TcpConnection(Socket&& sock);
    ~TcpConnection() = default;
    TcpConnection() = default;
    void operator() () { request(); }
    //处理监听到的HTTP请求
    void request();

    void serveFile(const std::string, const std::string);
    void executeCgi();
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
    std::string search_;
    int content_len;

};
