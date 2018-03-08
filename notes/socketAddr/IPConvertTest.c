#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <bits/sockaddr.h>
#include <sys/errno.h>
int main(int argc,char **argv)
{
    unsigned char strIP[INET_ADDRSTRLEN];  
    int i = 0;
    int size = 4;
    unsigned char addrIP[size];

    printf("Please input an IP\n");
    for(i=0;i < INET_ADDRSTRLEN;i++)  //使用scanf("%s",strIP);更好
    {
        scanf("%c",strIP+i);
        if(*(strIP+i) == '\n')
        {
            *(strIP+i) = '\0';
            break;
        }
    }
//注意这里的strIP必须是c风格的字符串，即最后一个字符必须以'\0'结尾
    if(inet_pton(AF_INET,strIP,addrIP) != 1)
    {
        if(errno == EAFNOSUPPORT)
          printf("No support Address Family!\n");
        printf("Convert Failed!\n");
    }
    else
    {
        printf("转换为二进制：");
        for(i=0;i < size;i++)
        {
            printf("%d",*(addrIP+i)); //注意是大端输出。
        }
        printf("\n");
        if(inet_ntop(AF_INET,addrIP,strIP,INET_ADDRSTRLEN) == NULL)
          printf("convert failed!\n");
        else
          printf("反转换结果为：%s",strIP);
    }
    return 0;
}
