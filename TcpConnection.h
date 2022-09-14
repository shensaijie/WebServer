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
    void *request();

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
    static constexpr const char* version_ = "HTTP/1.0";
    static constexpr const char* stat_ok = " 200 OK\r\n";
    static constexpr const char* server_ = "Server: ssj\r\n";
    static constexpr const char* type_ = "Content-Type: text/html\r\n";
    static constexpr const char* notfound_ = "<HTML><TITLE>Not Found</TITLE>\r\n<BODY><P>The server could not fulfill\r\nyour request because the resource specified\r\nis unavailable or nonexistent.\r\n</P></BODY></HTML>\r\n";

};
