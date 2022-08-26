#include "Error.h"
#include "config.h"

#include <sys/socket.h>
#include <arpa/inet.h>

void *accept_request(void* client);


class Server{
	private:	
		int server_sock = 0, client_sock = -1;
		u_short port = 0;
		struct sockaddr_in client_name;
		socklen_t client_name_len = sizeof(client_name);
	public:
		Server();
		Server(u_short port);
		void startup();
		void loop();
		u_short getPort();
		int getServerSock();
};

