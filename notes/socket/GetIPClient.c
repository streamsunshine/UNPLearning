/*********************************
 * 功能：列出从服务器获取的本机IP以及本机自己获取的IP
 * *****************************/
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/errno.h>

int main(int args,char *argv[])
{
    int dataSize;   //需要读取数据大小
    int readedSize; //已读取数据大小
    int wroteSize;  //已写入数据大小
    char buffer[INET_ADDRSTRLEN];   //读取数据缓存区
    struct sockaddr_in servAddr,localAddr;
    int returnValue;
    char addr[4];
    int sockfd;

    if(args != 2)
    {
        printf("You should input IP address!\n");
        exit(-1);
    }
    if(inet_pton(AF_INET,argv[1],addr) <= 0)  //将输入的点分十进制地址转换
    {
        printf("Please check out your address!");
        exit(-1);
    }
    bzero(&servAddr,sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = *(in_addr_t *)addr;
    servAddr.sin_port = htons(1);  //这里指定的端口号可能不符合常用端口号的要求
    if((sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == -1)  //创建套接字
    {
      printf("Socket descriptor allocate is wrong!");
      exit(-1);
    }

    if(connect(sockfd,(struct sockaddr *)&servAddr,sizeof(servAddr)) < 0)   //和服务器连接
    {
        printf("Can not connect with server,errno is :%d\n",errno);
       if(errno == ECONNREFUSED)
          printf("NO PROCESS!");
        if(errno == ETIMEDOUT)
          printf("Time out!\n");
        if(errno == EHOSTUNREACH || errno == ENETUNREACH)
          printf("Unreachable!");
        exit(-1);
    }
    dataSize = 1;
    readedSize = 0;
     //由于点分十进制IP地址大小不定，先通过一次通信获取大小信息
    while(readedSize < dataSize)      
    {
        if((returnValue = read(sockfd,buffer+readedSize,dataSize - readedSize)) == -1)
        {
            printf("Something wrong when read data from socket!");
            exit(-1);
        }
        readedSize += returnValue;
    }
    //将读到数据写回，用于将两次读数据分开
    dataSize = 1;
    wroteSize = 0;
    while(wroteSize < dataSize)
    {
        if((returnValue = write(sockfd,buffer+wroteSize,dataSize - wroteSize)) == -1)
        {
            printf("Write back Wrong!\n");
            exit(-1);
        }
        wroteSize += returnValue;
    }

    if(*buffer > INET_ADDRSTRLEN)
    {
        printf("The data is too long");
        exit(-1);
    }
    dataSize = *buffer;
    readedSize = 0;
    while(readedSize < dataSize)    //读取字符串的IP地址信息并输出
    {
        if((returnValue = read(sockfd,buffer+readedSize,dataSize - readedSize)) == -1)
        {
            printf("Something wrong when read data from socket!");
            exit(-1);
        }
        readedSize += returnValue;
    }
    printf("The server return IP address:\n");  
    printf("%s\n",buffer);

    //读取本地地址信息并输出
    dataSize = sizeof(localAddr);
    getsockname(sockfd,(struct sockaddr *)&localAddr,&dataSize);
    if(inet_ntop(AF_INET,&(localAddr.sin_addr.s_addr),buffer,INET_ADDRSTRLEN) == NULL)
    {
        printf("IP address converts failes\n!");
        exit(-1);
    }

    printf("The local address is:\n");
    printf("%s\n",buffer);
}


