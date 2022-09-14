#pragma once
#include "TcpConnection.h"
#include "InetAddress.h"
#include "Acceptor.h"
#include <iostream>
using namespace std;

typedef unique_ptr<TcpConnection> TcpConnectionPtr;


class TcpServer {
public:
void test_request() { 
    InetAddress seraddr("192.168.225.130", 8848);
    Acceptor acc(seraddr);
    while(1) {
	TcpConnectionPtr tcp = acc.accept();
	cout << "建立链接..." << tcp->fd() << endl;
	tcp->request();
    }
}

};
