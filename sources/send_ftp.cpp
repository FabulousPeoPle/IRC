#include <string>
#include <sys/socket.h>
# include <arpa/inet.h>
# include "../IRC_bot/utils.hpp"

#define END_STRING "\r\n"
struct sockaddr_in  serv_addr;


int m_send(int sock, std::string msg)
{
    int bytesSent = 0;
    msg += END_STRING;
    int size = msg.size();

    while (size)
    {
        bytesSent = send(sock, msg.data() + bytesSent, size, 0);
        if (bytesSent == -1)
            return (-1);
        size -= bytesSent;
    }
    return (0);
}

int m_send(int sock, char *msg, int lenght)
{
    int bytesSent = 0;
    int size = lenght;

    while (size)
    {
        bytesSent = send(sock, msg + bytesSent, size, 0);
        if (bytesSent == -1)
            return (-1);
        size -= bytesSent;
    }
    m_send(sock, END_STRING);
    return (0);
}

int m_recv(int sock, std::queue<std::string> &msg)
{
    char buffer[1024];
    int bytesRcev = 0;
	
    bytesRcev = recv(sock, buffer, 1023, 0);
    if (bytesRcev == 0)
        return (0);
	buffer[bytesRcev] = '\0';
    ssplit(std::string(buffer), END_STRING, msg);
    return (0);
}

int fileSender(int sock, std::string fileName)
{
    char *buffer;
    int len;
    FILE *img = fopen(fileName.c_str(), "rb");
    
    if (img == NULL)
    {
        std::cout << "could not open the file\n";
        return (-1);
    }
	fseek(img, 0, SEEK_END);
	len = ftell(img);
    rewind(img);

    buffer = (char *)malloc(sizeof(char) * len + 1);
    buffer[len] = '\0';
    fread(buffer, sizeof(char), len + 10, img);
    std::cout << buffer << std::endl;
    std::cout << "buffer size " << len + 1 << std::endl;
    m_send(sock, buffer, len);
    fclose(img);
    std::cout << "file sended" << std::endl;
    return 0;
}

int fileReciever(int sock, std::string msg)
{
    std::string fileName;
    std::vector<std::string> s_msg;

    ssplit(msg, " ", s_msg);
    fileName = s_msg.back();
    std::cout << "receving " << fileName << std::endl;
}

void m_cmdHandler(int sock, std::string msg, std::string fileName)
{
	// t_msg p_msg;
	int err = 0;
    // std::string msgCopy = msg;

	if ((msg.find("902") != std::string::npos))
        fileSender(sock, fileName);
	if (((msg.find("900") != std::string::npos)))
        fileReciever(sock, msg);
	    // err = m_cmdDispatcher(p_msg);
    // m_channelSearch(msg);
}

int m_qHandler(int sock, std::queue<std::string> &msg_q, std::string fileName)
{
    std::string msg;
    while (msg_q.empty() != true)
    {
        msg = msg_q.front();
        std::cout << msg << std::endl;
	 	msg_q.pop();
        m_cmdHandler(sock, msg, fileName);
    }
	return 0;
}

void listner(int sock, std::string fileName)
{
    std::queue<std::string> msg;
    while(1)
    {
        std::cout << "listn" <<std::endl;
        m_recv(sock, msg);
        if (msg.empty())
            break;
        m_qHandler(sock, msg, fileName);
    }
}

int auth(int sock, std::string pswd, std::string id)
{
    std::string user = "USER " + id + " * * : hello World";
    std::string nick = "NICK " + id;
    int err = 0;
    std::string passwd = "PASS " + pswd;

    if ((err = m_send(sock , user)))
        return err;
    if ((err = m_send(sock, nick)))
        return err;
    if ((err = m_send(sock, passwd)))
        return err;
	return 0;
}

#include <limits>
#include <iostream>
#include <fstream>

int sendNotice(int sock, std::string to_who, std::string fileName)
{
    int len;
    FILE *img = fopen(fileName.c_str(), "rb");
    if (img == NULL)
    {
        std::cout << "could not open the file\n";
        return (-1);
    }
	fseek(img, 0, SEEK_END);
	len = ftell(img);
    rewind(img);
    std::cout << "size of file : " << len << std::endl;
    send(sock, "SEND ", 5, 0);
    send(sock, to_who.c_str(), to_who.length(), 0);
    send(sock, " ", 1, 0);
    send(sock, fileName.c_str() , fileName.length(), 0);
    send(sock, " ", 1, 0);
    send(sock, std::to_string(len).c_str(), std::to_string(len).length(), 0);
    send(sock, "\r\n", 2, 0);
    fclose(img);
    return 0;
}

int startSender(std::string host, int port, std::string passwd, std::string fileName)
{
    int sock;

    host = host;
    port = port;
    passwd = passwd;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if(inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Connection Failed");
        return -1;
    }
    auth(sock, passwd, "ftpSender");
    sendNotice(sock, "ftpRecevier", fileName);
    listner(sock, fileName);
    return 0;
}

int startReceiver(std::string host, int port, std::string passwd, std::string fileName)
{
    int sock;

    host = host;
    port = port;
    passwd = passwd;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if(inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Connection Failed");
        return -1;
    }
    auth(sock, passwd, "ftpRecevier");
    // sendNotice(sock, dest, fileName);
    listner(sock, fileName);
    return 0;
}

int main(int ac, char *av[])
{
    if (ac!=6)
    {
        std::cout << "usage : ./ftp_client host port passwd filename dest/src 0/1" << std::endl;
        return 1;
    }
    if (std::string(av[5]) == "0")
        startSender(av[1], atoi(av[2]), av[3], av[4]);
    if (std::string(av[5]) == "1")
        startReceiver(av[1], atoi(av[2]), av[3], av[4]);
    return 0;
}