#pragma once
#include "Common.h"
#include "InetAddress.h"
#include "Socket.h"

#include <memory>

class TcpServer;

class TcpConnection {
    friend class TcpServer;
public:
    TcpConnection(Socket&& sock);
    ~TcpConnection() = default;
    TcpConnection() = default;

    void startUp();
    void Loop();
    //处理监听到的HTTP请求
    void request();

    void serveFile();
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
