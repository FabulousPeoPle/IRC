
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
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
    printf("%ld\n", send(sock , auth , strlen(auth) , 0 ));
	bool lock = true;
	while (true)
	{
	    read(sock, buffer, 1024);
	    printf("%s\n",buffer );
		if (lock)
		{
			int bytesSent = 0;
			send(sock, "SEND user0 img_copy.jpeg 46606\r\n", strlen("SEND user0 img_copy.jpeg 46606\r\n"), 0);
			FILE *img = fopen("../img.jpeg", "rb");
			unsigned char *buff = (unsigned char*)malloc(sizeof(unsigned char) * 46606);
			fread(buff, sizeof(unsigned char), 46606, img);
			read(sock, buffer, 1024);
	    	printf("%s\n",buffer );
			while (bytesSent < 46606)
			{
				bytesSent += send(sock, buff + bytesSent, 46606 - bytesSent, 0);
			}
			lock = false;
		}
	}
    // printf("Hello message sent\n");
    // valread = read( sock , buffer, 1024);
    return 0;
}