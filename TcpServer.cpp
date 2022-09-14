#include "TcpServer.h"
#include "Acceptor.h"

#include <iostream>
#include <pthread.h>


void TcpServer::startUp() { 
    InetAddress seraddr("192.168.225.130", 8848);
    Acceptor acc(seraddr);
    pthread_t pt;
    while(1) {
	    TcpConnectionPtr tcp = acc.accept(), tcp2;
        std::cout << "建立链接..." << tcp->fd() << std::endl;
        //tcp->request();        
        auto p = tcp.release();
        p->request();
        //using namespace std;
        
        //cout << "post " << p->post_ << "\ncgi " << p->cgi_ << "\npath " << p->path_ << endl;
        
        //tcp2.reset(tcp.release());
	    //pthread_create(&pt, NULL, tcp->TcpConnection::request, NULL);
        //tcp->request();
    }
}

void* TcpServer::requestHelper(void* args) {
    //((TcpServer*)args)->ptc->request();
    return NULL;
}
