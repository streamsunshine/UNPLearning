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

const unsigned int MAXLEN = 100;
const int portNum = 7;

void str_serv(FILE *fd,int sockfd);
void sigChildHandler(int sig);

int main()
{
    int sockfd,connectfd;
    struct addrinfo hints,*result,*tmpAddrinfoPtr;
    int reValue;
    int n;

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

    signal(SIGCHLD, sigChildHandler);

    while(1)
    {
        if((connectfd = accept(sockfd,NULL,NULL)) < 0)
        {
            if(errno == EINTR)
              continue;
        }
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

void  sigChildHandler(int sig)
{
    pid_t pid;    //store the pid of ended child process

    while((pid = waitpid(-1,NULL,WNOHANG)) > 0)    //pid为0时，说明没有退出的进程可收集
      printf("The child process %d end\n",pid);
    
    return;
}
