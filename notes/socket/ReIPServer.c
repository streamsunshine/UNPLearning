/******************************************
 * 功能：和客户机交互，并返回客户端请求的IP
 *****************************************/
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

int main(int argc,char *argv[])
{
    int sockfd;
    int connectSockfd;
    struct sockaddr_in servAddr,clientAddr;
    char buf[INET_ADDRSTRLEN];
    char len;
    char clientLen;
    int dataSize;
    int addrLen;
    int finiSize;
    int returnValue;

    if((sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == -1)
    {
        printf("Socket descriptor allocate id wrong!");
        exit(-1);
    }

    bzero(&servAddr,sizeof(servAddr));
    servAddr.sin_family = AF_INET;     //错误1与此处有关 
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(1);
    if((bind(sockfd,(struct sockaddr *)&servAddr,sizeof(servAddr))) == -1) //这里出现错误1
    {
        printf("Bind Failed\n");
        exit(-1);
    }

    if(listen(sockfd,20) < 0)
      printf("Listen failed!");
    while(1)
    {
        addrLen = sizeof(clientAddr);
        if((connectSockfd = accept(sockfd,(struct sockaddr *)&clientAddr,&addrLen)) == -1) //出过错
        {   
            printf("Connection failed!");
            exit(-1);
        }
    
        if(inet_ntop(AF_INET,&(clientAddr.sin_addr.s_addr),buf,INET_ADDRSTRLEN) == NULL)
        {
            printf("Address converts failed!");
            close(connectSockfd);
            exit(-1);
        }
    
        len = strlen(buf) + 1;
        dataSize = 1;
        finiSize = 0;
        while(finiSize < dataSize)
        {
            if((returnValue = write(connectSockfd,&len,dataSize - finiSize)) == -1)  //出错
            {
                close(connectSockfd);
                exit(-1);
            }
            finiSize += returnValue;
        }

        dataSize = 1;
        finiSize = 0;
     //由于点分十进制IP地址大小不定，先通过一次通信验证信息
        while(finiSize < dataSize)      
        {
            if((returnValue = read(connectSockfd,&clientLen,dataSize - finiSize)) == -1)
            {
                printf("Something wrong when read data from socket!");
                close(connectSockfd);
                exit(-1);
            }
            finiSize += returnValue;
        }   
        if(len != clientLen)
        {
            printf("Verify Failed!%d != %d",len,clientLen);
            close(connectSockfd);
            exit(-1);
        }   
    
        dataSize = strlen(buf) + 1;
        finiSize = 0;
        while(finiSize < dataSize)
        {
            if((returnValue = write(connectSockfd,buf+finiSize,dataSize - finiSize)) == -1)
            {
                close(connectSockfd);
                exit(-1);
            }
            finiSize += returnValue;
        }
        close(connectSockfd);
    }
}
