#include <sys/socket.h> //socket function
#include <unistd.h>     //fork,exec,read,write
#include <netinet/in.h> //seq bytes adjust,socket address
#include <arpa/inet.h>  //inet_pton,inet_ntop
#include <strings.h>    //bzero
#include <stdio.h>      //printf
#include <sys/errno.h>  //errno
#include <stdlib.h>     //exit
#include <string.h>     //strlen
#include <sys/select.h> //select
#include <sys/time.h>   //struct timeval

#define max(a,b) a>b?a:b


const int portNum = 9601;
void str_cli(FILE *fp,int sockfd);

int main(int args,char **argv)
{
    int sockfd;
    struct sockaddr_in servAddr;
    const char *defaultAddr = "127.0.0.1";

    if(args != 2)
    {
        printf("Use default address 127.0.0.1\n");
    }
    else
    {
        defaultAddr = argv[1];
    }

    if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
    {
        printf("socketfd wrong");
        exit(-1);
    }

    bzero(&servAddr,sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(portNum);
    inet_pton(AF_INET,defaultAddr,&servAddr.sin_addr.s_addr);
    
    if(connect(sockfd,(struct sockaddr *)&servAddr,sizeof(servAddr)) < 0)
    {
        printf("Can not connect to server\n");
        exit(-1);
    }

    str_cli(stdin,sockfd);
    printf("The client is shut!");
}

const unsigned int MAXLEN = 100;

void str_cli(FILE *fp,int sockfd)
{
    char inStr[MAXLEN];
    char wrStr[MAXLEN];
    unsigned int curLen;
    int n; 
  while(1)
  {
    if((n = read(fileno(fp),inStr,MAXLEN)) <= 0)
    {
        printf("process end!");
        return;
    }

    if((n = write(sockfd,inStr,n)) <= 0)
    {
        printf("Write Wrong!");
        return;
    }
    if((n = read(sockfd,wrStr,MAXLEN)) <= 0)
    {
        printf("read Wrong!");
        return;
    } 
    if((n = write(fileno(fp),wrStr,n)) <= 0)
    {
        printf("read Wrong!");
        return;
    } 
  }
}

