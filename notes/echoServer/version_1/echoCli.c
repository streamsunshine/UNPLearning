#include <sys/socket.h> //socket function
#include <unistd.h>     //fork,exec,read,write
#include <netinet/in.h> //seq bytes adjust,socket address
#include <arpa/inet.h>  //inet_pton,inet_ntop
#include <strings.h>    //bzero
#include <stdio.h>      //printf
#include <sys/errno.h>  //errno
#include <stdlib.h>     //exit
#include <string.h>     //strlen

const int portNum = 9600;
void str_cli(FILE *fp,int sockfd);

int main(int args,char **argv)
{
    int sockfd;
    struct sockaddr_in servAddr;

    if(args != 2)
    {
        printf("Invalid server address");
        exit(-1);
    }

    if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        printf("socketfd wrong");
        exit(-1);
    }

    bzero(&servAddr,sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(portNum);
    inet_pton(AF_INET,argv[1],&servAddr.sin_addr.s_addr);
    
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
    unsigned int curLen;
    int n; 
    unsigned int sendNum = 0;
    
    while(fgets(inStr,MAXLEN,fp))
    {
        sendNum = 0;

        curLen = strlen(inStr);
    
        while(sendNum < curLen) //make sure all data was written
        {
            n = write(sockfd,inStr + sendNum,curLen - sendNum);
            if(n < 0)
            {
                if(errno == EINTR)
                {   
                    n = 0;      //清零，防止后面影响发送计数
                   continue;
                }
                else
                {
                    printf("Write to socket wrong!");
                    exit(-1);
                }
            }
            if(n == 0)
                break;
            sendNum += n;
        }
        if(sendNum != curLen)
            printf("This data can not send completely!\n");
        n = read(sockfd,inStr,MAXLEN);
        if(n <= 0)      //read after write can not be EOF
        {
          printf("Read from server failed!");
          exit(-1);
        }
        if(fputs(inStr,stdout) == EOF)
        {
            printf("fputs error!");
            exit(-1);
        }

    }
}
