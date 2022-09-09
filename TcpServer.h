#pragma once
#include "Common.h"
#include "InetAddress.h"
#include "Socket.h"

#include <memory>
//typedef std::unique_ptr<TcpServer> TcpServerPtr;

class TcpServer : noncopyable {
public:
    TcpServer(Socket&& sock);
    ~TcpServer() = default;

    //处理监听到的HTTP请求
    void *request();

    void serveFile();

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
    static constexpr const char* version_ = "HTTP/1.0";
    static constexpr const char* stat_ok = " 200 OK\r\n";
    static constexpr const char* headers_ = "Server: ssj\r\nContent-Type: text/html\r\n\r\n";
    static constexpr const char* notfound_ = "<HTML><TITLE>Not Found</TITLE>\r\n<BODY><P>The server could not fulfill\r\nyour request because the resource specified\r\nis unavailable or nonexistent.\r\n</P></BODY></HTML>\r\n";

};
