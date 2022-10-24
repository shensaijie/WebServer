#pragma once
#include "Common.h"
#include "InetAddress.h"
#include "Socket.h"

#include <memory>

class TcpConnection;
typedef std::unique_ptr<TcpConnection> TcpConnectionPtr;

class TcpConnection {
public:
     enum METHOD { GET = 0, POST = 1, HEAD, PUT, DELETE, 
         TRACE, OPTIONS, CONNECT, PATCH };
     enum CHECK_STATE { CHECK_STATE_REQUESTLINE = 0, CHECK_STATE_HEADER, 
         CHECK_STATE_CONTENT };
     enum HTTP_CODE { NO_REQUEST = 0, GET_REQUEST, BAD_REQUEST, NO_RESOURCE, 
         FORBIDDEN_REQUEST, FILE_REQUEST, INTERNAL_ERROR, CLOSED_CONNECTION };
     enum LINE_STATUS { LINE_OK = 0, LINE_BAD, LINE_OPEN };    

private:
     // 解析http
     HTTP_CODE parseRequestLine(std::string&);
     HTTP_CODE parseHeaders(std::string&);

public:
    TcpConnection(Socket&& sock);
    ~TcpConnection() = default;
    void operator() () { request(); }
    //处理监听到的HTTP请求
    void request();

    void serveFile(const std::string, const std::string);
    void executeCgi();
    //recv send 发送接受数据
    int receiveAll(void* buf, int len);
    int receiveSome(void* buf, int len); 
    LINE_STATUS receiveLine(std::string&);  //读取一行，并返回行状态

    int sendAll(const void* buf, int len);
    int sendSome(const void* buf, int len);
    
    int fd() { return sock_.fd(); }

private:
    Socket sock_;        // 封装的套接字
    bool cgi_;           // 是否开启动态解析
    std::string path_;   // URL地址
    std::string search_; // get请求的search
    int content_len;     // 
    
    LINE_STATUS line_status;
    CHECK_STATE check_state;
    METHOD method_;      // 请求方式

    std::string send_buf;
};
