#include "Socket.h"
#include "InetAddress.h"
#include "Acceptor.h"
#include "TcpServer.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <unistd.h>
using namespace std;

typedef unique_ptr<TcpServer> TcpServerPtr;

void test_request() { 
    InetAddress seraddr("192.168.1.10", 8848);
    Acceptor acc(seraddr);
    while(1) {
	TcpServerPtr tcp = acc.accept();
	cout << "建立链接..." << tcp->fd() << endl;
	tcp->request();
    }
}


int main() {
    test_request();
    return 0;
}

void test_Socket(){
    Socket ser(0);
    ser = ser.createTCP(AF_INET);
    cout << "初始化套接字: " <<  ser.fd() << endl;
    ser.setReuseAddr(true);
    
    InetAddress addr("192.168.1.10", 8848);
    ser.bind(addr);
    ser.listen();
    

    while(1) {
	cout << "监听中。。。" << endl;
	int cli = accept(ser.fd(), nullptr, nullptr);
	if(cli != -1){
	    cout << "new connection..." << cli << endl;
	    send(cli, "Socket test", 12, 0);
	}
    }
}

void test_Acceptor() {
    InetAddress seraddr("192.168.1.10", 8848);
    Acceptor acc(seraddr);
    while(1) {
	Socket client_socket = acc.acceptSocket();
	cout << "建立链接..." << client_socket.fd() << endl;
    } 
}

void test_TcpServer() {
    
    InetAddress seraddr("192.168.1.10", 8848);
    Acceptor acc(seraddr);
    while(1) {
	TcpServerPtr tcp = acc.accept();
	cout << "建立链接..." << tcp->fd() << endl;
	char buff[1024];
	tcp->receiveSome(buff, sizeof buff);
	buff[1023] = '\0';
	cout << buff << endl;
	string buf = "HTTP/1.0 200 OK\r\n";
	buf += "ssj server\r\n";
	buf += "Content-Type: text/html\r\n";
	buf += "\r\n";
	buf += "<html><title>Hello World</title><body><h1>Hello World!</h1></body></html>";
	tcp->sendAll(buf.c_str(), buf.size());
	break;
    }
}

