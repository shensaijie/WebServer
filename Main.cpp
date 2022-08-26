#include "config.h"
#include "Server.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>


int main()
{
	Server server((u_short)8848);
	server.startup();
	 
	printf("http server_sock is %d\n", server.getPort());
        printf("http running on port %d\n", server.getServerSock());
	server.loop();
	
	return 0;
}




