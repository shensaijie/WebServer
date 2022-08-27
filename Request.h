#include <iostream>
#include <cstring>
#include <sys/stat.h>

#define ISspace(x) isspace((int)(x))


class Request{
	private:	
	 	int client = -1;
	 	char buf[1024];
		int numchars;
		char method[255];
		char url[255];
		char path[512];
		struct stat st; 
		int cgi = 0;     
		char *query_string = NULL;
		void cat(int, FILE*);
		int get_line(int, char *, int);
		void headers(int, const char *);
	public:
		Request(int *client) { this->client = *client;}
		void request();
		void execute_cgi(int, const char *, const char *,const char *); 
		void serve_file(int, const char *);
};
