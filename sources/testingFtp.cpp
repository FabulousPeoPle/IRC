
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fcntl.h>
#define PORT 6667


void    grabStrayData(int sock, unsigned long & readbytes , unsigned long & length, char * buffer, unsigned char * content)
{
    char * ptr;
    char  ptr0[2];
    char c;
    int i = 0;

    std::cout << "grabbing data\n";
    length = strtoul(buffer + 7, NULL, 10);
    ptr = strstr(buffer, "|");
    if (ptr && strlen(ptr) >= 3)
    {
        std::cout << "sector1\n";
        ptr += 3;
        std::cout << "sector1 point 1\n";
        while (*ptr)
        {
            std::cout << "sector1 looping\n";
            content[i] = *ptr;
            i++;
            ptr++;
        }
        readbytes = i;
        return ;
    }
    while (c != '|')
    {
        read(sock, &c, 1);
    }
    read(sock, ptr0, 2);
    std::cout << "sector1\n";
}

int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if (argc != 4)
    {
        std::cout << "USAGE: ./ftpTester [target user] [file name] [1/2(send/recieve)]\n";
        return -1;
    }
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
    if (std::string(argv[3]) == std::string("1"))
    
    {
        send(sock , auth , strlen(auth) , 0 );
        std::cout << "sender\n";
    }
    else
    {
        send(sock , auth2 , strlen(auth2) , 0 );
        std::cout << "reciever\n";
    }
    std::string fileName = argv[2];
    std::string target = argv[1];


    // fcntl(sock, F_SETFL, O_NONBLOCK);


	bool lock = true;
    bool waitfor = true;
    unsigned long readbytes = 0;
    unsigned long readbytes_s = 0;
    int bytesSent = 0;
    unsigned long len = 0;

	while (true)
	{
	    recv(sock, buffer, 2048, 0);
	    //printf("%s\n",buffer );
		if (lock && std::string(argv[3]) == std::string("1"))
		{
			sleep(3);
            FILE *img = fopen(fileName.c_str(), "rb");
            if (img == NULL)
            {
                std::cout << "could not open the file\n";
                return (-1);
            }
	        fseek(img, 0, SEEK_END);
	        len = ftell(img);
            rewind(img);
            
			unsigned char *buff = (unsigned char*)malloc(sizeof(unsigned char) *(len +1 ));
            buff[len] = '\0';
			fread(buff, sizeof(unsigned char), len, img);
			recv(sock, buffer, 2048, 0);
	    	//printf("--->%s\n", buffer );
            while (true)
            {
                send(sock, "SEND ", 5, 0);
                send(sock, target.c_str(), target.length(), 0);
                send(sock, " ", 1, 0);
                send(sock, fileName.c_str() , fileName.length(), 0);
                send(sock, " ", 1, 0);
                send(sock, std::to_string(len).c_str(), std::to_string(len).length(), 0);
                send(sock, "\r\n", 2, 0);
                sleep(1);
                std::cout << "--->waiting\n";
                readbytes = recv(sock, buffer, 100, 0);
                buffer[readbytes] = '\0';
                //printf("LE BUFFER IS--->%s\n",buffer );
                if (strstr(buffer, "902"))
                {
                    //std::cout << "--->recieved code\n";
                    break ;
                }
                readbytes = 0;
            }
            std::cout << "this is the buff we sending: " << buff << std::endl;
			while (bytesSent < len)
			{
                std::cout << "--->sending " << bytesSent << std::endl;
				bytesSent += send(sock, buff + bytesSent, len - bytesSent, 0);
                std::cout << "--->sent " << bytesSent << std::endl;
			}
			fclose(img);
		}
        else if (lock && std::string(argv[3]) == std::string("2"))
        {
            readbytes = 0;
            int recievedBytes = 0;
            char * code = NULL;
            char * buff_useless;
            len = 219009;
            unsigned char *buff_send = (unsigned char*)malloc(sizeof(unsigned char) *(len +1 ));
            bzero(buff_send, len);
            buff_send[len] = '\0';
            
            while (true)
            {
                send(sock, "FETCH\r\n", 7, 0);
                sleep(1);
                std::cout << "--->waiting\n";
                readbytes = recv(sock, buffer, 100, 0);
                buffer[readbytes] = '\0';
                printf("LE BUFFER IS--->%s\n",buffer );
                
                if ((buff_useless = strstr(buffer, "905")))
                {
                    std::cout << "inner sector\n";
                    while  (!code)
                    {
                        code = strstr(buffer, "length ");
                        if (code)
                        {
                            grabStrayData(sock, readbytes_s, len, code, buff_send);
                            break ;
                        }
                        readbytes = recv(sock, buffer, 300, 0);
                        buffer[readbytes] = '\0';
                        std::cout << "------->" << buffer << std::endl;
                    }
                    std::cout << "--->recieved code and length " << std::to_string(len) << std::endl;
                    break ;
                }
            }
            int rec = 0;
            std::cout << "taking file\n";
            // std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
            send(sock, "YES", 3, 0);
            sleep(1);
            while (readbytes_s < len)
			{
                std::cout << "---> recieving \n";
				rec = recv(sock, buff_send + readbytes_s, len - readbytes_s, 0);
                if (rec < 0)
                {
                    perror("recv: ");
                    break ;
                }
                readbytes_s += rec;
                std::cout << "---> recieved " << readbytes_s << std::endl;
			}
            std::cout << buff_send << std::endl;
            FILE *dest = fopen((fileName).c_str(), "wb");
            int i = 0;
            std::cout << "before printing in file : lenght " << len << " readbytes : " << readbytes_s << "\n";
            while (i < len)
	        {
		        fputc(buff_send[i], dest);
		        i++;
	        }
            std::cout << "lenght " << len << " recieved " << i << "\n";
            fclose(dest);
        }
        lock = false;
	}
    return 0;
}