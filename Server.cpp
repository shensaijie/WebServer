#include "Server.h"
#include "Request.h"
#include <cstdio>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <cstring>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>



Server::Server(){
	this->port = 0;
}

Server::Server(u_short port){
	this->port = port;
}

u_short Server::getPort(){
	return port;
}

int Server::getServerSock(){
	return server_sock;
}

void Server::startup()
{
	int &httpd = server_sock;
	int option;
	struct sockaddr_in name;
	//初始化套接字	
	httpd = socket(PF_INET, SOCK_STREAM, 0);

	//设置socket属性
	socklen_t optlen;
	optlen = sizeof(option);
	option = 1;
	//通用套接字SOL_SOCKET,允许套接口和一个已在使用中的地址捆绑
	setsockopt(httpd, SOL_SOCKET, SO_REUSEADDR, (void *)&option, optlen);

	memset(&name, 0, sizeof(name));
	name.sin_family = AF_INET;//地址簇
	name.sin_port = htons(port);//16位端口号
	name.sin_addr.s_addr = htonl(INADDR_ANY);//32位ip地址，INADDR_ANY本机所有ip

	
	//绑定端口、ip到socket上
	 if (bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0)
	 {
	 	error_die("bind");
	 }
	 if (port == 0) //动态分配端口号
	 {
	 	socklen_t  namelen = sizeof(name);
 	if (getsockname(httpd, (struct sockaddr *)&name, &namelen) == -1)
	   		error_die("getsockname");
	  	port = ntohs(name.sin_port);
	 }

	 //开启监听
	 if (listen(httpd, 5) < 0)
		 error_die("listen");
}

void Server::loop(){
	pthread_t newthread;
	while (1)
	{
		client_sock = accept(server_sock, (struct sockaddr *)&client_name, &client_name_len);
		  
		printf("New connection....  ip: %s , port: %d\n",inet_ntoa(client_name.sin_addr),ntohs(client_name.sin_port));
		if (client_sock == -1)	error_die("accept");
		if (pthread_create(&newthread , NULL, accept_request, (void*)&client_sock) != 0)
			perror("pthread_create");

	}
	close(server_sock);
}

