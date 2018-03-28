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


const int portNum = 9600;
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

    if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
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
    unsigned int sendNum = 0;
    fd_set readSet;     //设置读监测集合
    int endOfTerminal = 0; //标志位，在解决问题二的同时，判断程序是正常关闭还出现异常

    FD_ZERO(&readSet);  //清零集合 FD相关的指令是宏定义的
    
    while(1)
    {
        if(endOfTerminal == 0)
          FD_SET(fileno(fp),&readSet);    //fileno求出描述符,FD_SET设置集合
        FD_SET(sockfd,&readSet);        //使用select函数，解决问题1

        if((n = select(max(fileno(fp),sockfd) + 1,&readSet,NULL,NULL,NULL)) <0)
        {
            if(errno == EINTR)
              continue;
        }
        
        if(FD_ISSET(sockfd,&readSet))
        {
            n = read(sockfd,inStr,MAXLEN);      //使用read读出数据解决问题三
            if(n < 0)      //read after write can not be EOF
            {
                printf("Read from server failed!");
                exit(-1);
            }
            if(n == 0)
            {
                if(endOfTerminal == 1)  //说明客户端在终端输入了EOF
                  return;
                printf("Can not get data from server!");
            }
            
            n = write(fileno(fp),inStr,n);
            if(n <= 0)
            {
                printf("Write to terminal wrong!");
                exit(-1);
            }


        }

        if(FD_ISSET(fileno(fp),&readSet))
        {
             n = read(fileno(fp),wrStr,MAXLEN);
            if(n < 0)      //read after write can not be EOF
            {
                printf("Read from terminal failed!");
                exit(-1);
            }
            if(n == 0)
            {
                shutdown(sockfd,SHUT_WR);   //关闭写，解决问题二
                endOfTerminal = 1;
                FD_CLR(fileno(fp),&readSet);    //不会再从终端读数据了，所以不再监测
                continue;
            }

            n = write(sockfd,wrStr,n);
            if(n <= 0)
            {
                printf("Write to socket wrong!");
                exit(-1);
            }
        }
    }
}
