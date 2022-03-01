
 #include <netdb.h>
 #include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
int main(void)
{

        struct addrinfo hints, *rp;
        struct addrinfo *servinfo;

        memset(&hints, '\0', sizeof(hints));
        hints.ai_flags = AI_CANONNAME;
        hints.ai_family = AF_INET;            // IPV4 or IPV6
        hints.ai_socktype = SOCK_STREAM;
        int ret = getaddrinfo("www.irs.gov", NULL, &hints, &servinfo);
        for (rp = servinfo; rp != NULL; rp = rp->ai_next)
        {

        printf("%d=== =%s\n", ret, rp->ai_canonname);
        }
    return 0;
}