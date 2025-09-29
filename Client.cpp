#include <iostream>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <thread>
#include <fstream>
using namespace std;

void stream(const char *ip, int port)
{
    int sock;
    struct sockaddr_in addr;
    socklen_t addr_size;
    char buffer[1024] = {0};
    int n;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1){
        cout << "Socket was not created" << endl;
        exit(-1);
    }
    cout << "Socket was succesfully made" << endl;

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    cout << "Connected to server" << endl;

    string url = "GET /Index.html HTTP/1.0\r\nHost: 127.0.0.1\r\n\r\n";
    cout << "Client: " << url << endl; 
    send(sock, url.c_str(), url.size(), 0);

    memset(&buffer, '\0', sizeof(buffer));
    int size = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if(size > 0){
        buffer[size] = '\0';
    }
    cout << "Server: " << buffer << endl;

    close(sock);
    cout << "Disconnected" << endl;
}

int main()
{
    const char *ip = "127.0.0.1";
    int port = 6678;
    
    stream(ip, port);
    cout << "Receiving the message" << endl;

    return 0;
}