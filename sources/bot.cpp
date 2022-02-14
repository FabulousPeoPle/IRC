
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
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
    char auth[] = "USER dhdkjdkfjhkasdfhjkasdhfjksdhfkjadhsfjkahsdjkfhasdfsdfasdfsdkjsdhfkjahdfklhjaksdjfdhfjkadhfjkadhfkajdhfklajdhfkjsdhfklajsdhfkljsdhfklajsdhfkladjshfklasjdhfklsajdhfklsjdfhklsjdfhkjsdfhkPINGfhaklsdjfhaklsdjfhaksldjfhaklsdjfhklsajdfhkjsadhfkljsdhfkjsdhflkadhfjksahdfkjPINGkjhsdkfjhkasdfhjkasdhfjksdhfkjadhsfjkahsdjkfhasdfsdfasdfsdfasdfdhfjkadhfjkadhfkajdhfklajdhfkjsdhfklajsdhfkljsdhfklajsdhfkladjshfklasjdhfklsajdhfklsjdfhklsjdfhkjsdfhkdjsfhkajdhfksajdfhaklsdjfhaklsdjfhaksldjfhaklsdjfhklsajdfhkjsadhfkljsdhfkjsdhflkadhfjksahdfkjPINGkjhsdkfjhkasdfhjkasdhfjksdhfkja^ * * : bot_john\r\n";
    char auth1[] = "* * :hey\r\n";
    printf("%ld\n", send(sock , auth , strlen(auth) , 0 ));
    // sleep(2);
    // send(sock , auth1 , strlen(auth1) , 0 );
    // sleep(2);
    // send(sock , auth1 , strlen(auth1) , 0 );


    read(sock, buffer, 1024);
    printf("%s\n",buffer );
    // printf("Hello message sent\n");
    // valread = read( sock , buffer, 1024);
    return 0;
}