#include "Request.h"
#include "Error.h"
#include "config.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

//  处理监听到的 HTTP 请求
void *accept_request(void* from_client)
{
	 int client = *(int *)from_client;
	 char buf[1024];
	 int numchars;
	 char method[255];
	 char url[255];
	 char path[512];
	 size_t i, j;
	 struct stat st; 
	 int cgi = 0;     
	 char *query_string = NULL;

	 numchars = get_line(client, buf, sizeof(buf));
	

	i = 0; 
	j = 0;
	 while (!ISspace(buf[j]) && (i < sizeof(method) - 1))
	 {
		 //提取其中的请求方式
		  method[i] = buf[j];
		  i++; 
		  j++;
	 }
	method[i] = '\0';

	 if (strcasecmp(method, "GET") && strcasecmp(method, "POST"))
	 {
		  unimplemented(client);
		  return NULL;
	 }

	 if (strcasecmp(method, "POST") == 0)  cgi = 1;
	  
	 i = 0;
	 while (ISspace(buf[j]) && (j < sizeof(buf)))
	  j++;


	 while (!ISspace(buf[j]) && (i < sizeof(url) - 1) && (j < sizeof(buf)))
	 {
		  url[i] = buf[j];
		  i++; j++;
	 }
	url[i] = '\0';


	//GET请求url可能会带有?,有查询参数
	 if (strcasecmp(method, "GET") == 0)
	 {
		 
		  query_string = url;
		  while ((*query_string != '?') && (*query_string != '\0'))
				query_string++;
		 
		  /* 如果有?表明是动态请求, 开启cgi */
		  if (*query_string == '?')
		  {
			   cgi = 1;
			   *query_string = '\0';
			   query_string++;
		  }


		 }

	 sprintf(path, "httpdocs%s", url);


	 if (path[strlen(path) - 1] == '/')
	 {
		 strcat(path, "index.html");

	 }
 
	if (stat(path, &st) == -1) {
		  while ((numchars > 0) && strcmp("\n", buf))  
		   numchars = get_line(client, buf, sizeof(buf));

		  not_found(client);
	}
	else
	{
	

		if ((st.st_mode & S_IFMT) == S_IFDIR)//S_IFDIR代表目录
	   //如果请求参数为目录, 自动打开test.html
		{
			strcat(path, "index.html");
		}
	
	  //文件可执行
	  if ((st.st_mode & S_IXUSR) ||
		  (st.st_mode & S_IXGRP) ||
		  (st.st_mode & S_IXOTH))
		  //S_IXUSR:文件所有者具可执行权限
		  //S_IXGRP:用户组具可执行权限
		  //S_IXOTH:其他用户具可读取权限  
			cgi = 1;

	  if (!cgi)

			serve_file(client, path);
	  else
	   execute_cgi(client, path, method, query_string);
  
	 }

	 close(client);
	 //printf("connection close....client: %d \n",client);
	 return NULL;
}
void cat(int client, FILE *resource)
{
	//发送文件的内容
	 char buf[1024];
	 fgets(buf, sizeof(buf), resource);
	 while (!feof(resource))
	 {

		  send(client, buf, strlen(buf), 0);
		  fgets(buf, sizeof(buf), resource);
	 }
}
//执行cgi动态解析
void execute_cgi(int client, const char *path,
                 const char *method, const char *query_string)
{


	 char buf[1024];
	 int cgi_output[2];
	 int cgi_input[2];
	 
	 pid_t pid;
	 int status;

	 int i;
	 char c;

	 int numchars = 1;
	 int content_length = -1;
	 //默认字符
	 buf[0] = 'A'; 
	 buf[1] = '\0';
	 if (strcasecmp(method, "GET") == 0)

		 while ((numchars > 0) && strcmp("\n", buf))
		 {
			 numchars = get_line(client, buf, sizeof(buf));
		 }
	 else    
	 {

		  numchars = get_line(client, buf, sizeof(buf));
		  while ((numchars > 0) && strcmp("\n", buf))
		  {
				buf[15] = '\0';
			   if (strcasecmp(buf, "Content-Length:") == 0)
					content_length = atoi(&(buf[16]));

			   numchars = get_line(client, buf, sizeof(buf));
		  }

		  if (content_length == -1) {
		   bad_request(client);
		   return;
		   }
	 }


	 sprintf(buf, "HTTP/1.0 200 OK\r\n");
	 send(client, buf, strlen(buf), 0);
	 if (pipe(cgi_output) < 0) {
		  cannot_execute(client);
		  return;
	 }
	 if (pipe(cgi_input) < 0) { 
		  cannot_execute(client);
		  return;
	 }

	 if ( (pid = fork()) < 0 ) {
		  cannot_execute(client);
		  return;
	 }
	 if (pid == 0)  /* 子进程: 运行CGI 脚本 */
	 {
		  char meth_env[255];
		  char query_env[255];
		  char length_env[255];

		  dup2(cgi_output[1], 1);
		  dup2(cgi_input[0], 0);


		  close(cgi_output[0]);//关闭了cgi_output中的读通道
		  close(cgi_input[1]);//关闭了cgi_input中的写通道


		  sprintf(meth_env, "REQUEST_METHOD=%s", method);
		  putenv(meth_env);

		  if (strcasecmp(method, "GET") == 0) {
		  //存储QUERY_STRING
		   sprintf(query_env, "QUERY_STRING=%s", query_string);
		   putenv(query_env);
		  }
		  else {   /* POST */
			//存储CONTENT_LENGTH
		   sprintf(length_env, "CONTENT_LENGTH=%d", content_length);
		   putenv(length_env);
		  }


		  execl(path, path, NULL);//执行CGI脚本
		  exit(0);
	 } 
	 else {  
		  close(cgi_output[1]);
		  close(cgi_input[0]);
		  if (strcasecmp(method, "POST") == 0)

			 for (i = 0; i < content_length; i++) 
			   {

				recv(client, &c, 1, 0);

				write(cgi_input[1], &c, 1);
			   }



		//读取cgi脚本返回数据

		while (read(cgi_output[0], &c, 1) > 0)
			//发送给浏览器
		{			
			send(client, &c, 1, 0);
		}

		//运行结束关闭
		close(cgi_output[0]);
		close(cgi_input[1]);


		  waitpid(pid, &status, 0);
	}
}
//解析一行http报文
int get_line(int sock, char *buf, int size)
{
	 int i = 0;
	 char c = '\0';
	 int n;

	 while ((i < size - 1) && (c != '\n'))
	 {
		  n = recv(sock, &c, 1, 0);

		  if (n > 0) 
		  {
			   if (c == '\r')
			   {

				n = recv(sock, &c, 1, MSG_PEEK);
				if ((n > 0) && (c == '\n'))
				recv(sock, &c, 1, 0);
				else
				 c = '\n';
			   }
			   buf[i] = c;
			   i++;
			   
		  }
		  else
		   c = '\n';
	 }
	 buf[i] = '\0';
	return(i);
}

void headers(int client, const char *filename)
{
	 
	 char buf[1024];

	 (void)filename;  /* could use filename to determine file type */
	//发送HTTP头
	 strcpy(buf, "HTTP/1.0 200 OK\r\n");
	 send(client, buf, strlen(buf), 0);
	 strcpy(buf, SERVER_STRING);
	 send(client, buf, strlen(buf), 0);
	 sprintf(buf, "Content-Type: text/html\r\n");
	 send(client, buf, strlen(buf), 0);
	 strcpy(buf, "\r\n");
	 send(client, buf, strlen(buf), 0);

}

//如果不是CGI文件，也就是静态文件，直接读取文件返回给请求的http客户端即可
void serve_file(int client, const char *filename)
{
	 FILE *resource = NULL;
	 int numchars = 1;
	 char buf[1024];
	 buf[0] = 'A'; 
	 buf[1] = '\0';
	 while ((numchars > 0) && strcmp("\n", buf)) 
	 {
		 numchars = get_line(client, buf, sizeof(buf));
	 }
	
	 //打开文件
	 resource = fopen(filename, "r");
	 if (resource == NULL)
	  not_found(client);
	 else
	 {
	  headers(client, filename);
	  cat(client, resource);
	 }
	 fclose(resource);//关闭文件句柄
}
