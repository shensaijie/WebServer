#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "Main.h"

int startup(u_short *port)
{
	int httpd = 0, option;
	struct sockaddr_in name;
	//初始化套接字	
	httpd = socket(PF_INET, SOCK_STREAM, 0);
	printf("httpd = %d\n", httpd);
	
	//设置socket属性
	socklen_t optlen;
	optlen = sizeof(option);
	option = 1;
	setsockopt(httpd, SOL_SOCKET, SO_REUSEADDR, (void *)&oprion, optlen);//通用套接字SOL_SOCKET,
		
	return httpd;
}

int main()
{
	int server_sock = -1, client_sock = -1;
	u_short port = 8848;//default port
	struct sockaddr_in client_name;
	socklen_t client_name_len = sizeof(client_name);
	pthread_t newthread;
	server_sock = startup(&port);
 
	printf("http server_sock is %d\n", server_sock);
        printf("http running on port %d\n", port);

	return 0;
}
