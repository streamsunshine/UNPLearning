#include <sys/errno.h>  //errno
#include <stdlib.h>     //exit
#include <strings.h>    //bzero
#include <string.h>     //strlen
#include <arpa/inet.h>  //inet_pton
#include <sys/socket.h> //socket function,socket connect
#include <netinet/in.h> //seq bytes adjust,socket address
#include <stdio.h>      //printf
#include <unistd.h>     //fork,exec,write
#include <sys/wait.h>     //waitpid
#include <netdb.h>
#include <pthread.h>    //pthread_xxx

const unsigned int MAXLEN = 100;
const int portNum = 7;

void *str_serv(void *sockfdPtr);

int main()
{
    int sockfd,connectfd;
    struct addrinfo hints,*result,*tmpAddrinfoPtr;
    int reValue;
    int n;
    pthread_t tid;

    //set server address
    bzero(&hints,sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;
    
    reValue = getaddrinfo(NULL,"echo",&hints,&result);

    if(reValue != 0)
    {
        printf("%s",gai_strerror(reValue));
        exit(-1);
    }

    tmpAddrinfoPtr = result;
    while(tmpAddrinfoPtr != NULL)
    {
        //get socket num
        if((sockfd = socket(tmpAddrinfoPtr->ai_family,tmpAddrinfoPtr->ai_socktype,tmpAddrinfoPtr->ai_protocol)) < 0)
        {
            continue;
        }
        //bind server address
        if((bind(sockfd,tmpAddrinfoPtr->ai_addr,tmpAddrinfoPtr->ai_addrlen))< 0)
        {
            close(sockfd);
            continue;
        }
        else
          break;

    }
    if(tmpAddrinfoPtr == NULL)
    {
        printf("create listen failed");
        exit(-1);
    }
    //set monitor sockfd
    if(listen(sockfd,20) < 0)   //20 is the sum of waiting queue and accepted queue
    {
        printf("Create listen failed");
        exit(-1);
    }

    freeaddrinfo(result);


    while(1)
    {
        if((connectfd = accept(sockfd,NULL,NULL)) < 0)
        {
            if(errno == EINTR)
              continue;
        }
        if(pthread_create(&tid,NULL,&str_serv,(void *)&connectfd) > 0)
        {
            printf("Create pthread failed!");
            exit(-1);
        }
    }
}

void *str_serv(void *sockfdPtr)
{
    int nreaded;
    char buffer[MAXLEN];
    int sockfd = *(int *)sockfdPtr;

    pthread_detach(pthread_self()); //设置线程为脱离的
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
        close(sockfd);
        return;
    }
}

