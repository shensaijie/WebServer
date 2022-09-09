#include "TcpServer.h"

#include <unistd.h>
#include <sys/stat.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

TcpServer::TcpServer(Socket&& sock) : sock_(std::move(sock)) { }

void* TcpServer::request() {
    std::string buf, word;
    while(receiveLine(buf)==0);
    std::istringstream in(buf);
    in >> word;
    if(::strcasecmp(word.c_str(), "GET") && ::strcasecmp(word.c_str(), "POST")) {
	//请求方式错误
	return nullptr;
    }
    
    if(::strcasecmp(word.c_str(), "POST") == 0)
	post_ = cgi_ = true;
    else 
	post_ = false;
    
    in >> word;

    //GET请求有可能带？
    if(!post_) {
	if(word.find('?') != std::string::npos)
	    cgi_ = true;
    }
    path_ = "httpdocs" + word;

    if(path_.back()=='/') path_ += "index.html";

    
    struct stat st;
    if (stat(path_.c_str(), &st) == -1) {	
        while(receiveLine(buf) && buf!="\n") buf = "";
	//返回没有找到
    } else {
	if((st.st_mode & S_IFMT) == S_IFDIR)//S_IFDIR代表目录
	    path_ += "/index.html";

	if((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH));
	else cgi_ = 0;
	    
    }
    if(cgi_)
	serveFile();
    else
	serveFile();

    return nullptr;
}

int TcpServer::receiveAll(void* buf, int len) {
    return ::recv(sock_.fd(), buf, len, MSG_WAITALL);
}

int TcpServer::receiveSome(void* buf, int len) {
    return sock_.recv(buf, len);
}

int TcpServer::receiveLine(std::string &str) {
    char c = '\0';
    while(c != '\n') {
	if(::recv(sock_.fd(), &c, 1, 0) > 0) {
	    if(c == '\r') {
		if(::recv(sock_.fd(), &c, 1, MSG_PEEK) > 0 && c=='\n')
		    ::recv(sock_.fd(), &c, 1, 0);
		else
		    c = '\n';
	    }
	    str += c;
	}
	else 
	    c = '\n';
    }
    return str.size();
}


int TcpServer::sendAll(const void* buf, int len) {
    int written = 0;
    while(written < len) {
	int nw = sock_.send(static_cast<const char*>(buf) + written, len - written);
	if(nw > 0) {
	    written += nw;
	} else {
	    break;
	}
    }
    return written; //返回写入的数量
}

int TcpServer::sendSome(const void* buf, int len) {
    return sock_.send(buf, len);
}

//如果不是CGI文件，也就是静态文件，直接读取文件返回给请求的http客户端即可
void TcpServer::serveFile() {
    std::string buf;
    std::fstream fin;
    while(receiveLine(buf) && buf!="\n") buf = "";
    fin.open(path_, std::ios_base::in);
    if(fin.is_open()) {
	sendAll(version_ , sizeof version_);
	sendAll(stat_ok, sizeof stat_ok);
	sendAll(headers_, sizeof headers_);
	
	while(fin.good()){
	    getline(fin, buf);
	    buf += "\r\n";
	    //std::cout << "(line " << ++n << ")" << buf << " " << buf.size();
	    sendAll(buf.c_str(), buf.size());
	}
    } else {
	//404
    }
    fin.close();
}
/*
//执行cgi动态解析
void execute_cgi(int client, const char *path, const char *method, const char *query_string)
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
	 if (pid == 0)  // 子进程: 运行CGI 脚本 
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
		  else {   // POST 
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
		{	//发送给浏览器	
			send(client, &c, 1, 0);
		}

		//运行结束关闭
		close(cgi_output[0]);
		close(cgi_input[1]);


		waitpid(pid, &status, 0);
	}
}

*/
