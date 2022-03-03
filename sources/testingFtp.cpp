
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#define PORT 6667

int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
       
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
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
    char auth[] = "USER user * * : bot_john\r\nNICK ftp1\r\nPASS 123456\r\n";
    char auth2[] = "USER user * * : bot_john1\r\nNICK ftp2\r\nPASS 123456\r\n";
	// if (argv[1] == "1")
    printf("--->%ld\n", send(sock , auth , strlen(auth) , 0 ));
	bool lock = true;
    bool waitfor = true;
    int readbytes = 0;
	while (true)
	{
	    read(sock, buffer, 2048);
	    printf("%s\n",buffer );
		if (lock)
		{
            std::cout << "lock is not locked\n";
			int bytesSent = 0;
			send(sock, "SEND user0 text 10\r\n", strlen("SEND user0 text 10\r\n"), 0);
			FILE *img = fopen("../img.jpeg", "rb");
			unsigned char *buff = (unsigned char*)malloc(sizeof(unsigned char) * 10 + 1);
            buff[10] = '\0';
			fread(buff, sizeof(unsigned char), 10, img);
			read(sock, buffer, 2048);
	    	printf("--->%s\n", buffer );
            while (true)
            {
                std::cout << "--->waiting\n";
                readbytes = recv(sock, buffer, 100, 0);
                buffer[readbytes] = '\0';
                printf("LE BUFFER IS--->%s\n",buffer );
                if (strstr(buffer, "902"))
                {
                    std::cout << "--->recieved code\n";
                    break ;
                }
                readbytes = 0;
            }
            std::cout << "this is the buff we sending: " << buff << std::endl;
			while (bytesSent < 10)
			{
                std::cout << "--->sending " << bytesSent << std::endl;
				bytesSent += send(sock, buff + bytesSent, 10 - bytesSent, 0);
                std::cout << "--->sent " << bytesSent << std::endl;
			}
			lock = false;
		}
	}
    // printf("Hello message sent\n");
    // valread = read( sock , buffer, 1024);
    return 0;
}