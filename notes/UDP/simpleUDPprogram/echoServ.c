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
#include <sys/select.h>   //select

const unsigned int MAXLEN = 100;
const int portNum = 9601;
const int MAXCLIENT = 100;

int str_serv(FILE *fd,int sockfd);

int main()
{
    int sockfd;
    struct sockaddr_in servAddr;
    int n;

    //set server address
    bzero(&servAddr,sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(portNum);

    //get socket num
    if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
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
    str_serv(stdin,sockfd);

}

int str_serv(FILE *fd,int sockfd)  //the fd parameter seems useless
{
    int nreaded;
    char buffer[MAXLEN];
    struct sockaddr_in cliAddr;
    int cliLen = sizeof(cliAddr);
    
  while(1)
  {
    nreaded = recvfrom(sockfd,buffer,MAXLEN,0,(struct sockaddr *)&cliAddr,&cliLen);
    if(nreaded < 0)
    {
        printf("Read from client Wrong!");
        exit(-1);
    }
    if(sendto(sockfd,buffer,nreaded,0,(struct sockaddr *)&cliAddr,cliLen) < 0)
    {
        printf("Server writes Wrong!");
        exit(-1);
    }
  }
    return 0;
}

