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

string parse_http(char buffer[], int length)
{
    string url(buffer, length); // this turns the c string into c++ string
    string filename;

    int end = url.find(' ');
    if(end == string::npos){
        cout << "Error with end in parse_http" << endl;
        exit(-1);
    }

    int filepath_end = url.find(' ', end + 1);
    if(filepath_end == string::npos){
        cout << "error with filepath_end" << endl;
        exit(-1);
    }

    filename = url.substr(end + 1,  filepath_end - end - 1);
    cout << filename << endl; // more debugging, it should look like /index.html

    if (filename == " "){
        return "Index.html"; // if the filename is empty, return what I know the filename is supposed to be
    }
    if(filename[0] == '/'){
        filename = filename.substr(1);
    } else {
        cout << "Error getting the filename" << endl;
    }

    return filename;
}

void data_handle(int client_sock)
{
    char buffer[1024] = {0};
    string message;

    recv(client_sock, buffer, sizeof(buffer), 0);
    cout << buffer << endl;
    int length = strlen(buffer);

    string filename = parse_http(buffer, length);
    ifstream the_file(filename);
    if(!the_file.is_open()){
        cout << "Couldn't open the file" << endl;
        message = "Error 404 lol";
        send(client_sock, message.c_str(), message.size(), 0);
        close(client_sock);
        return;
    }
    cout << "The file was opened!" << endl; // just for debugging

    string file_content((istreambuf_iterator<char>(the_file)), istreambuf_iterator<char>()); 
    // this should read the entire file into the string


    string size = to_string(file_content.size());
    message = "HTTP/1.0 200 OK \r\n"
              "Content-Type: text/html\r\n"
              "Content-Length: " + size + "\r\n\r\n"
               + file_content; 


    send(client_sock, message.c_str(), message.size(), 0);
    close(client_sock);

    cout << "Request was from " << filename << endl;
}

void stream(const char *ip, int port)
{
    int server_sock;
    int client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[1024] = {0};
    int n;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock == -1){
        cout << "Error making socket" << endl;
        exit(-1);
    }
    cout << "Socket was created" << endl;

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    n = ::bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)); 
    // apparently the :: tells the compiler to use the global bind instead of namespace std bind
    if(n == -1){
        cout << "Couldn't bind socket" << endl;
        exit(-1);
    }
    cout << "Successfully binded the socket to port" << endl;
    
    listen(server_sock, 5);
    cout << "Listening..." << endl;

    while(1){
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        if(client_sock == -1){
            cout << "Error in the accept function" << endl;
            exit(-1);
        }
        cout << "Client connected" << endl;

        thread t(data_handle, client_sock);
        t.detach();
    }
}

int main()
{
    const char *ip = "127.0.0.1";
    int port = 6789;
    
    stream(ip, port);
    cout << "Sending the message" << endl;

    return 0;
}
