#include "Acceptor.h"
#include "InetAddress.h"
#include "TcpConnection.h"

#include <iostream>
#include <thread>


void requestHelper(TcpConnectionPtr tcpConnection) {
    tcpConnection->request();
}

int main () { 
    InetAddress listenAddr(8848);
    Acceptor acceptor(listenAddr);
    std::cout << "Listen on port 8848\n";
    std::cout << "Accepting ... Ctrl-C to exit\n";
    
    while(1) {
	    TcpConnectionPtr tcpConnection = acceptor.accept();
        std::cout << "new connection... port:" << tcpConnection->fd() << std::endl;
    
        //创建新线程处理请求
        std::thread thr(requestHelper, std::move(tcpConnection));
        thr.detach();
    }
}

