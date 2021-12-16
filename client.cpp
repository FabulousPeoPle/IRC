


#include <sys/socket.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include <iostream>

#include <sys/types.h>
#include <netdb.h>
#include <string.h>

#define PORT 6667

int main(int argc, char **argv)
{
    int status;
    int sockfd;
    struct sockaddr_in serv;

    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    inet_pton(AF_INET, argv[1], &(serv.sin_addr));

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Bruh, socket creation failed\n");
        exit(1);
    }

    if ((connect(sockfd, (struct sockaddr *) &serv, sizeof serv)) < 0)
    {
        printf("failed to connect\n");
        exit(1);
    }
    send(sockfd, (std::string("PASS ") + std::string(argv[2])).c_str() + std::string("\r\n"), 7 + strlen(argv[2]), 0);
    // send(sockfd, (std::string("othmane:") + std::string(argv[2])).c_str(), 8 + strlen(argv[2]), 0);
    printf("sent");
    return (0);
}