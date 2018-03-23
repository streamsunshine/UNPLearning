#include <sys/errno.h>  //errno
#include <stdlib.h>     //exit
#include <strings.h>    //bzero
#include <string.h>     //strlen
#include <arpa/inet.h>  //inet_pton
#include <sys/socket.h> //socket function,socket connect
#include <netinet/in.h> //seq bytes adjust,socket address
#include <stdio.h>      //printf
#include <unistd.h>     //fork,exec,write

const unsigned int MAXLEN = 100;
const int portNum = 9600;

void str_serv(FILE *fd,int sockfd);

int main()
{
    int sockfd,connectfd;
    struct sockaddr_in servAddr;
    int n;

    //set server address
    bzero(&servAddr,sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(portNum);

    //get socket num
    if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        printf("Socketfd wrong!");
        exit(-1);
    }

    //bind server address
    if((bind(sockfd,(struct sockaddr *)&servAddr,sizeof(servAddr))) < 0)
    {
        printf("Bind failed!");
        exit(-1);
    }

    //set monitor sockfd
    if(listen(sockfd,20) < 0)   //20 is the sum of waiting queue and accepted queue
    {
        printf("listen failed!");
        exit(-1);
    }

    while((connectfd = accept(sockfd,NULL,NULL)) > 0)
    {
        if((n = fork()) < 0)
        {
            printf("Create process child failed!");
            exit(-1);
        }
        else if(n == 0)
        {
            str_serv(stdout,connectfd);
            close(connectfd);
            exit(0);
        }
        else
        {
            close(connectfd);
        }
    }
    if(connectfd < 0)
    {
        printf("Accept wrong!");
    }
}
void str_serv(FILE *fd,int sockfd)  //the fd parameter seems useless
{
    int nreaded;
    char buffer[MAXLEN];

again:
    while((nreaded = read(sockfd,buffer,MAXLEN)) > 0)
    {
        write(sockfd,buffer,nreaded);
    }

    if(nreaded < 0)
    {
        if(errno == EINTR)
          goto again;
        else
        {
            printf("Server read wrong!");
            exit(-1);
        }
    }
    else        //nreaded equal to zero,client input EOF
    {
        return;
    }
}

