#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <limits>
#include <iostream>
#include <fstream>
#include "ssplit.hpp"

#define END_STRING "\r\n"
struct sockaddr_in  serv_addr;
void m_cmdHandler(int sock, std::string msg, std::queue<std::string> &msg_q, std::string &fileName);
int fileRecv(int sock, std::string fileName, std::string msg, std::queue<std::string> &msg_q);

void fileWrite(std::string fileName, char *data)
{
    struct stat st = {0};
    FILE * logFile;

    if ((logFile = fopen(fileName.c_str(), "w")) == NULL)
    {
        std::cout << "Can not open/create file" << std::endl;
        exit(1);
    }
    fputs(data, logFile);
    fclose(logFile);
}

int m_send(int toFd, std::string message)
{
    int bytesSent = 0;
    message += END_STRING;
    int size = message.size();
    int total = 0;

    while (size)
    {
        bytesSent = send(toFd, message.data() + total, size, 0);
        if (bytesSent == -1)
        {
            perror("Send");
            return (-1);
        }
        total += bytesSent;
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
    buffer = (char *)malloc(sizeof(char) * len);
    fread(buffer, sizeof(char), len, img);
    std::cout << "buffer size " << len << std::endl;
    m_send(sock, buffer, len);
    fclose(img);
    std::cout << "file sended" << std::endl;
    return 0;
}
int m_recv(int sock, std::queue<std::string> &msg)
{
    char buffer[1024];
    int bytesRcev = 0;
	
    bytesRcev = recv(sock, buffer, 1024, 0);
    if (bytesRcev == 0)
        return (0);
    ssplit(std::string(buffer), END_STRING, msg);
    return (0);
}

int recv_data(int sock, std::queue<std::string> &msg)
{
    char buffer[1024];
    int bytesRcev = 0;
	size_t pos = 0;  
	std::string token1;
	
    bytesRcev = recv(sock, buffer, 1024, 0);
    if (bytesRcev == 0)
        return (0);
  
    std::string str = buffer;
    if((pos = str.find(END_STRING)) != std::string::npos)
    {
        token1 = str.substr(0, pos);
        msg.push(token1);

        str.erase(0, pos + 2);
    }
	if (str.size() != 0)
		msg.push(str);
    return (0);
}

int m_qHandler(int sock, std::string &msg, std::queue<std::string> &msg_q, std::string &fileName)
{
    while (msg_q.empty() != true)
    {
        msg = msg_q.front();
	 	msg_q.pop();
        m_cmdHandler(sock, msg, msg_q, fileName);
    }
	return 0;
}

int m_qHandler(int sock, std::queue<std::string> &msg_q, std::string &fileName)
{
    std::string msg;
    while (msg_q.empty() != true)
    {
        msg = msg_q.front();
	 	msg_q.pop();
        m_cmdHandler(sock, msg, msg_q, fileName);
    }
	return 0;
}

std::string fileFetcher(int sock, std::string msg)
{
    std::string fileName;
    std::vector<std::string> s_msg;
    std::queue<std::string> q_msg;

    ssplit(msg, " ", s_msg);
    fileName = s_msg.back();
    std::cout << "Fetching " << fileName << std::endl;
    send(sock, "FETCH\r\n", 7, 0);
    recv_data(sock, q_msg);
    fileRecv(sock, fileName, msg, q_msg);
    return fileName;
}

void m_cmdHandler(int sock, std::string msg, std::queue<std::string> &msg_q, std::string &fileName)
{
	// t_msg p_msg;
	int err = 0;
    // std::string msgCopy = msg;

	if ((msg.find("902") != std::string::npos))
        fileSender(sock, fileName);
	if (((msg.find("900") != std::string::npos)))
        fileName = fileFetcher(sock, msg);
}

int fileRecv(int sock, std::string fileName, std::string msg, std::queue<std::string> &msg_q)
{
    std::vector<std::string> s_msg;
    char buffer[1024];
    int count;
    int lenght;
    std::ofstream wf(fileName, std::ios::out | std::ios::binary);

    if(!wf)
    {
        std::cout << "Cannot open file!" << std::endl;
        return -1;
    }
    std::cout << "Receving " << fileName << std::endl;
    ssplit(msg_q.front(), " ", s_msg);
    std::cout << msg_q.front() << std::endl;
    msg_q.pop();
    lenght = atoi(s_msg[9].c_str());
    count = msg_q.size();
    int a = 0;
    for (int i = 0; i < count; i++)
    {
        wf.write(msg_q.front().c_str(), strlen(msg_q.front().c_str()));
        a += strlen(msg_q.front().c_str());
        msg_q.pop();
    }
    int byteread = 0;
    long rem = 0;
    if (a < lenght - 1)
    {
        rem = lenght - a;
        // std::cout << "data rem : " << count << std::endl;
        while (rem)
        {
            memset(buffer, 0, 1024);
            byteread = recv(sock, buffer, 1024, 0);
            if (byteread == -1)
            {
                std::cout << "readn't" <<std::endl;
                continue;
            }
            std::cout << "data read : " << byteread << std::endl;
            rem -= byteread;
            wf.write(buffer, byteread);
            std::cout << "data rem : " << rem << std::endl;
        }
    }
    std::cout << "file received" << std::endl;
    wf.close();
    return 0;
}



void listner(int sock, std::string fileName)
{
    std::queue<std::string> msg;
    while(1)
    {
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
    auth(sock, passwd, "ftpS");
    sendNotice(sock, "ftpR", fileName);
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
    auth(sock, passwd, "ftpR");
    listner(sock, fileName);
    return 0;
}

int main(int ac, char *av[])
{
    if (ac == 5 && std::string(av[4]) == "1")
    {
        startReceiver(av[1], atoi(av[2]), av[3], av[4]);
        return 0;
    }
    else if (ac == 6 && std::string(av[5]) == "0")
    {
        startSender(av[1], atoi(av[2]), av[3], av[4]);
        return 0;
    }
    std::cout << "send usage : ./ftp_client host port passwd 0 fileName" << std::endl;
    std::cout << "receive usage : ./ftp_client host port passwd 1" << std::endl;
    return 1;
}