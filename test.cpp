#include "Socket.h"
#include "InetAddress.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
using namespace std;


void test_socket(){
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

int main() {
    test_socket();
    return 0;
}
