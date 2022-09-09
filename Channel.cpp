#include "Channel.h"
#include <string>

std::string& Channel::run() {
    std::string buf = "HTTP/1.0 200 OK\r\n";
    buf += "ssj server\r\n";
    buf += "Content-Type: text/html\r\n";
    buf += "\r\n";
    buf += "<html><title>Hello World</title><body><h1>Hello World!</h1></body></html>";
    
    return buf;
}
