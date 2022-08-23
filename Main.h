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

#define ISspace(x) isspace((int)(x))

#define SERVER_STRING "Server: Shen Saijie's http/0.1.0\r\n"//定义个人server名称

int get_line(int, char *, int);

void cat(int, FILE *);

void headers(int, const char *);

void serve_file(int, const char *);

int startup(u_short *);

void *accept_request(void* client);

void execute_cgi(int, const char *, const char *, const char *);

void bad_request(int);

void cannot_execute(int);

void error_die(const char *);

void not_found(int);

void unimplemented(int);
