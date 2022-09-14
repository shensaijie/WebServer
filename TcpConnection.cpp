#include "TcpConnection.h"

#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

TcpConnection::TcpConnection(Socket&& sock) : sock_(std::move(sock)) { }


void *TcpConnection::request() {
    std::cout << "request\n";
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
        int n = word.find('?');
        if(word.size() > n)
            search_ = word.substr(n+1, word.size()-n-1);
        word = word.substr(0, n);
    }
    path_ = "httpdocs" + word;

    if(path_.back()=='/') path_ += "index.html";

    
    struct stat st;
    if (stat(path_.c_str(), &st) == -1) {	
        while(receiveLine(buf) && buf!="\n") buf = "";
        //返回没有找到
    } else {
	if((st.st_mode & S_IFMT) == S_IFDIR)//S_IFDIR代表目录
	    path_ += "index.html";

	if((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH));
	else cgi_ = 0;
	    
    }
    if(cgi_)
	    executeCgi();
    else
    	serveFile();

    return nullptr;
}

int TcpConnection::receiveAll(void* buf, int len) {
    return ::recv(sock_.fd(), buf, len, MSG_WAITALL);
}

int TcpConnection::receiveSome(void* buf, int len) {
    return sock_.recv(buf, len);
}

int TcpConnection::receiveLine(std::string &str) {
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


int TcpConnection::sendAll(const void* buf, int len) {
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

int TcpConnection::sendSome(const void* buf, int len) {
    return sock_.send(buf, len);
}

//如果不是CGI文件，也就是静态文件，直接读取文件返回给请求的http客户端即可
void TcpConnection::serveFile() {
    std::string buf;
    std::fstream fin;
    while(receiveLine(buf) && buf!="\n") buf = "";
    fin.open(path_, std::ios_base::in);
    if(fin.is_open()) {
    buf = version_;
    buf += stat_ok;
    buf += server_;
    buf += type_;
    buf += "\r\n";
    sendAll(buf.c_str(), buf.size());
	while(fin.good()){
	    getline(fin, buf);
	    sendAll(buf.c_str(), buf.size());
	}
        sendAll("\r\n", 2);
    } else {
        //404
    }
    fin.close();
}

void TcpConnection::executeCgi() {
     std::string buf;
     int cgi_output[2];
	 int cgi_input[2];
	 int status;

	 if (post_ == 0)//GET方式
         while(receiveLine(buf) && buf!="\n") buf = "";
	 else    
	 {
          content_len = -1;
          while(receiveLine(buf) && buf!="\n") {
              if(buf.compare(0, 14, "Content-Length")==0)
                  content_len = std::stoi(buf.substr(16, buf.size()-16));
              else if(buf=="\r\n")
                  break;
              buf = "";
          }
		  if (content_len == -1) {
		     //bad_request
		     return;
		  }
	 }

     buf = version_;
     buf += stat_ok;
     sendAll(buf.c_str(), buf.size());
	 if (pipe(cgi_output) < 0) {
		  //cannot_execute
		  return;
	 }
	 if (pipe(cgi_input) < 0) { 
		  //cannot_execute
		  return;
	 }

     pid_t pid;
	 if ( (pid = fork()) < 0 ) {
		  //cannot_execute(client);
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

		  sprintf(meth_env, "REQUEST_METHOD=%s", post_ ? "POST" : "GET");
		  putenv(meth_env);

		  if (post_ == 0) {
		  	//存储QUERY_STRING
		   	sprintf(query_env, "QUERY_STRING=%s", search_.c_str());
		   	putenv(query_env);
		  }
		  else {   // POST 
			//存储CONTENT_LENGTH
		  	sprintf(length_env, "CONTENT_LENGTH=%d", content_len);
		   	putenv(length_env);
		  }
          
		  execl(path_.c_str(), path_.c_str(), NULL);//执行CGI脚本
		  exit(0);
	 } 
	 else {  
		  char c;
          close(cgi_output[1]);
		  close(cgi_input[0]);
		  if (post_)
			 for(int i = 0; i < content_len; i++) {
				 receiveAll(&c, 1);
				 write(cgi_input[1], &c, 1);
			 }
		//读取cgi脚本返回数据
		while (read(cgi_output[0], &c, 1) > 0) {//发送给浏览器	
			sendAll(&c, 1);
		}

		//运行结束关闭
		close(cgi_output[0]);
		close(cgi_input[1]);


		waitpid(pid, &status, 0);
	}
}


