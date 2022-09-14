#include "TcpServer.h"
#include "Acceptor.h"

#include <iostream>
#include <pthread.h>


void TcpServer::startUp() { 
    InetAddress seraddr("192.168.225.130", 8848);
    Acceptor acc(seraddr);
    pthread_t pt;
    while(1) {
	    TcpConnectionPtr tcp2 = acc.accept();
        std::cout << "new connection... port:" << tcp2->fd() << std::endl;
        //auto p = tcp.release();
        //p->request();
        
        tcp = tcp2.release();
	    //tcp->request();
        pthread_create(&pt, NULL, &TcpServer::requestHelper, this);
    }
}

void* TcpServer::requestHelper(void* args) {
    ((TcpServer*)args)->tcp->request();
    return NULL;
}
