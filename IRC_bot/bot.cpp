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

#define CLIENT_ADDR "0.0.0.0" 
#define PORT 6667
using namespace std;

std::string strToken(std::string str, std::string delim);

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

int ssplit(string str, string delim, queue<string> &msg)
{

    size_t pos = 0;  
    string token1; // define a string variable  
  
    // use find() function to get the position of the delimiters  
    while ((pos = str.find(delim)) != std::string::npos)  
    {
        token1 = str.substr(0, pos); // store the substring
        // cout << "token >>> " + token1 << endl;
        msg.push(token1);

        str.erase(0, pos + delim.length());  /* erase() function store the current positon and move to next token. */
    }
    // cout << "split remain:  " + str + "||"<< endl;
    return 0;
}

int m_recv(int toFd, queue<string> &msg)
{
    char buffer[1024];
    int bytesRcev = 0;
    string s;

    bytesRcev = read(toFd, buffer, 1023);
    if (bytesRcev == 0)
        return (0);
	buffer[bytesRcev] = '\0';
    ssplit(string(buffer), END_STRING, msg);
    return (0);
}

typedef struct s_msg
{
	std::string user;
	std::string nick;
	std::string addr;
	std::string cmd;
	std::string args;
	std::string msg;
}t_msg;


int msg_parse(std::string msg)
{
	t_msg p_msg;
	cout << "checking cmd  : " << msg << endl;
	if (msg.find("!bota ") != std::string::npos)
	{
		std::cout << "valid bot cmd : " << msg << endl;

		if ((p_msg.nick = strToken(msg.erase(0, 1), "!")).size() == 0)
			return 1;
		cout << "nick >> " + p_msg.nick << endl;
	}
	return 0;
}

void cmd_handler(string msg)
{
	t_msg p_msg;
	msg_parse(msg);
	if (msg.compare(0, 6, "!bota ") == 0)
		cout << "good cmd" << endl;
}




int q_handler(queue<string> &msg_q)
{
    string msg;

    while (msg_q.empty() == false)
    {
        msg = msg_q.front();
        msg_q.pop();
		cmd_handler(msg);
        cout << "f queue >> " + msg << endl;
    }
	return 0;
}

bool auth(int sock, string user, string nick)
{
    std::string auth = "USER " + user + " * * : " + user + " is here" + END_STRING + "NICK " + nick;
	return m_send(sock, auth);
}

bool ch_join(int sock, string channel_name)
{
    std::string msg = "JOIN #" + channel_name;
	return m_send(sock, msg);
}

bool lusers(int sock)
{
    std::string msg = "LUSERS";
	return m_send(sock, msg);
}
int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    queue<string> msg;
    // Bot bot;

	if (argc != 3)
	{
		std::cout << "usage : bota [ip] [port]" << endl;
		return 1;
	}
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) 
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
	auth(sock, "bota", "bota"); // returns 0 when working.
	ch_join(sock, "ch1");
    while(1)
    {
        m_recv(sock, msg);
        q_handler(msg);
        // cout << msg.front() << endl;
        // cout << "nl" << endl;
        // char buffer[1024] = {0};
        // scanf("%s", buffer);
        // auth = buffer;
        // auth += END_STRING;
        // m_send(sock, auth);
        // read(sock, buffer, 1024);
        // printf("%s\n",buffer );
    }

    // send(sock , auth1 , strlen(auth1) , 0 );


    // printf("Hello message sent\n");
    // valread = read( sock , buffer, 1024);
    return 0;
}
