// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <queue>

#include  <iostream>
// #include "bot.hpp"

#define END_STRING "\r\n"

#define CLIENT_ADDR "50.56.86.206" 
#define PORT 6667
using namespace std;



int m_send(int toFd, std::string message)
{
    int bytesSent = 0;
    message += END_STRING;
    int size = message.size();

    while (size)
    {
        // TODO: What the last parameter?
        bytesSent = send(toFd, message.data() + bytesSent, size, 0);
        if (bytesSent == -1)
            return (-1);
        size -= bytesSent;
    }
    return (0);
}

int ssplit(string str, string delim, queue<string> *msg)
{

    size_t pos = 0;  
    string token1; // define a string variable  
  
    // use find() function to get the position of the delimiters  
    while ((pos = str.find(delim)) != std::string::npos)  
    {
        token1 = str.substr(0, pos); // store the substring
        // cout << "token >>> " + token1 << endl;
        msg->push(token1);

        str.erase(0, pos + delim.length());  /* erase() function store the current positon and move to next token. */
    }
    // cout << "split remain:  " + str + "||"<< endl;
    return 0;
}

int m_recv(int toFd, queue<string> *msg)
{
    char buffer[1024];
    int bytesRcev = 0;
    string s;

    bytesRcev = read(toFd, buffer, 1024);
    if (bytesRcev == 0)
        return (0);
    ssplit(string(buffer), END_STRING, msg);
    cout << "q >>> " + msg->back() << endl;
    return (0);
}

using sysclock_t = std::chrono::system_clock;

std::string CurrentTime()
{
    std::time_t now = sysclock_t::to_time_t(sysclock_t::now());

    char buf[22] = { 0 };
    std::strftime(buf, sizeof(buf), "%T %Z %d/%m/%y", std::localtime(&now));
    
    return std::string(buf);
}

int m_send_prv(int sock, string channel_name, string msg)
{
    m_send(sock, "PRIVMSG" + channel_name + ":" + "It's "+ CurrentTime());

    return 0;
}

int cmd_handler(int sock, string msg)
{
    //TODO : extract recipient name 
    if (msg.compare(0, 6, "!bota ") == 0)
    {
        if (msg.compare(6, 5, "time") == 0)
        {
            msg = CurrentTime();
            string channel_name = "test";
            m_send_prv(sock, channel_name, msg);
        }
        else if (msg.compare(6, 5, "mok") == 0)
        {
            msg = "mok ya mok"
            string channel_name = "test";
            m_send_prv(sock, channel_name, msg);
        }

    }    
    return 0;
}

int q_handler(int sock, queue<string> *msg_q)
{
    string msg;

    while (msg_q->empty() == false)
    {
        msg = msg_q->front();
        cmd_handler(sock, msg);
        msg_q->pop();
    }
    return 0;
}

int auth(int sock)
{
    std::string auth = "USER bota * * : bota greets you";
    std::string auth1 = "NICK bota";

    m_send(sock, auth);
    m_send(sock, auth1);
}

int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    queue<string> msg;
    // Bot bot;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, CLIENT_ADDR, &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    printf("\nConnection GOOD \n");

    auth(sock);
    cout << "entring the loop" << endl;
    while(1)
    {
        m_recv(sock, &msg);
        q_handler(sock, &msg);
    }
    return 0;
}
