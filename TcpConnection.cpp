#include "TcpConnection.h"

#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>

const std::string version_( "HTTP/1.0");
const std::string stat_ok( " 200 OK\r\n");
const std::string server_("Server: ssj\r\n");
const std::string type_("Content-Type: text/html\r\n");
const std::string notfound_("<HTML><TITLE>Not Found</TITLE>\r\n<BODY><P>The server could not fulfill\r\nyour request because the resource specified\r\nis unavailable or nonexistent.\r\n</P></BODY></HTML>\r\n");


TcpConnection::TcpConnection(Socket&& sock) : 
    sock_(std::move(sock)), cgi_(0), path_(""), 
    search_(""), content_len(0), send_buf(""),
    check_state(CHECK_STATE_REQUESTLINE), method_(GET)  { }

TcpConnection::HTTP_CODE 
TcpConnection::parseRequestLine(std::string &szTemp) {
    std::vector<std::string> tmp;
    std::string::size_type pos;
    int size = szTemp.size();
    
    for (int i = 0; 1;) {
        while(szTemp[i] == ' ' && i < size) ++i; 
        if (i >= size) break;
        pos = szTemp.find(" ", i);
        if (pos < size) {
            std::string s = szTemp.substr(i, pos - i);
            tmp.push_back(s);
            i = pos + 1;
        } else {
            tmp.push_back(szTemp.substr(i, size - i));
            break;
        }
    }
    
    if ( tmp.size() != 3) {
        return BAD_REQUEST;
    }

    if( tmp[0] == "GET") {
        method_ = GET;
    } else if ( tmp[0] == "POST") {
        method_ = POST;
        cgi_ = true;
    } else {
        return BAD_REQUEST;
    }

    std::string &word = tmp[1];
    // GET请求有可能带？，需要解析
    if ( method_ == GET) {
	    if(word.find('?') != std::string::npos)
	        cgi_ = true;
            int n = word.find('?');
            if(word.size() > n)
                search_ = word.substr(n+1, word.size()-n-1);
        word = word.substr(0, n); 
    }

    path_ = "httpdocs" + word;
    if(path_.back()=='/') path_ += "index.html";

    if( tmp[2] != "HTTP/1.0" && tmp[2] != "HTTP/1.1")
        return BAD_REQUEST;

    return NO_REQUEST;
}

TcpConnection::HTTP_CODE TcpConnection::parseHeaders(std::string &buf) {
    while( true ){
        if ((line_status = receiveLine(buf)) == TcpConnection::LINE_BAD) {
            return BAD_REQUEST;
        }
        else if (line_status == LINE_OK) {
            if( buf == "" ) break;
            if(buf.compare(0, 14, "Content-Length")==0)
                content_len = std::stoi(buf.substr(16, buf.size()-16));
            if(content_len == -1)
                return BAD_REQUEST;
            buf = "";
         }
    }
    return GET_REQUEST;
}

void TcpConnection::request() {
    std::string buf, word;
    if( (line_status = receiveLine(buf)) != TcpConnection::LINE_OK){
        if (line_status == TcpConnection::LINE_BAD)
            return;
    }
    if( parseRequestLine(buf) != NO_REQUEST )
        return;

    if ( parseHeaders(buf) != GET_REQUEST)
        return;

    struct stat st;
    if (stat(path_.c_str(), &st) == -1) {	
        std::cout << "没有找到页面：" << path_ << std::endl;
        std::string stat(" 404 Not Found\r\n");
        path_ = "httpdocs/not_found.html";
        serveFile(stat, path_);
        return;
    } else {
	    if((st.st_mode & S_IFMT) == S_IFDIR)//S_IFDIR代表目录
	        path_ += "index.html";

	    if((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) 
                || (st.st_mode & S_IXOTH));
	        else cgi_ = 0;
    }

    if(cgi_)
	    executeCgi();
    else
    	serveFile(stat_ok, path_);

    return;
}

int TcpConnection::receiveAll(void* buf, int len) {
    return ::recv(sock_.fd(), buf, len, MSG_WAITALL);
}

int TcpConnection::receiveSome(void* buf, int len) {
    return sock_.recv(buf, len);
}

TcpConnection::LINE_STATUS TcpConnection::receiveLine(std::string &str) {
    char c = '\0';
    while ( true ) {
        int ret = ::recv(sock_.fd(), &c, 1, 0);
	    if(ret > 0) {
	        if(c == '\r') {
		        if(::recv(sock_.fd(), &c, 1, MSG_PEEK) > 0 && c=='\n') {
		            ::recv(sock_.fd(), &c, 1, 0);
                    return TcpConnection::LINE_OK;
                }
		        else
		            return TcpConnection::LINE_BAD;
	        }
	    }
	    else if (ret == 0)
            return TcpConnection::LINE_OPEN;
        else if (ret < 0)
            return TcpConnection::LINE_BAD;
        str += c;
    }
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
void TcpConnection::serveFile(const std::string stat, const std::string fpath) {
    std::string buf;
    std::fstream fin;

    fin.open(fpath, std::ios_base::in);
    if(fin.is_open()) {
        buf = version_ + stat + server_ + type_ + "\r\n";
        sendAll(buf.c_str(), buf.size());
	    while(fin.good()){
	        getline(fin, buf);
	        sendAll(buf.c_str(), buf.size());
	    }
        sendAll("\r\n", 2);
    } else {
        // 系统错误
        std::cout << "没有打开" << fpath << std::endl;
    }
    fin.close();
    return;
}

void TcpConnection::executeCgi() {
     std::string buf;
     int cgi_output[2];
	 int cgi_input[2];
	 int status;

     buf = version_;
     buf += stat_ok;
     sendAll(buf.c_str(), buf.size());
	 if (pipe(cgi_output) < 0) {
		  //cannot_execute
          std::cout << "cannot_execute\n";
		  return;
	 }
	 if (pipe(cgi_input) < 0) { 
		  //cannot_execute
          std::cout << "cannot_execute\n";
		  return;
	 }

     pid_t pid;
	 if ( (pid = fork()) < 0 ) {
		  //cannot_execute(client);
          std::cout << "cannot_execute(client)\n";
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

		  sprintf(meth_env, "REQUEST_METHOD=%s", method_ ? "POST" : "GET");
		  putenv(meth_env);

		  if (method_ == GET) {
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
		  if (method_ == POST)
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


