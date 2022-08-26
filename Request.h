#include <iostream>
#include <cstring>

#define ISspace(x) isspace((int)(x))

void *accept_request(void* from_client);
void cat(int, FILE *);
void execute_cgi(int, const char *, const char *, const char *);
int get_line(int, char *, int);
void headers(int, const char *);
void serve_file(int, const char *);
