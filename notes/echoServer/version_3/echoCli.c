#include <sys/socket.h> //socket function
#include <unistd.h>     //fork,exec,read,write
#include <netinet/in.h> //seq bytes adjust,socket address
#include <arpa/inet.h>  //inet_pton,inet_ntop
#include <strings.h>    //bzero
#include <stdio.h>      //printf
#include <sys/errno.h>  //errno
#include <stdlib.h>     //exit
#include <string.h>     //strlen
#include <netdb.h>      //DNS,addr-name,name-addr

void str_cli(FILE *fp,int sockfd);

int main(int args,char **argv)
{
    int sockfd;
    struct addrinfo hints,*result = NULL;
    struct addrinfo *tmpAddrinfoPtr = NULL;
    int reValue;

    bzero(&hints,sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if(args != 2)       //如果用户忘记输入地址，使用默认地址
    {
        printf("Use localhost!\n");
        reValue = getaddrinfo("localhost","echo",&hints,&result);
    }
    else
    {
        reValue = getaddrinfo(argv[1],"echo",&hints,&result);
    }

    if(reValue != 0)
    {
        printf("%s",gai_strerror(reValue));
        exit(-1);
    }

    tmpAddrinfoPtr = result;
    while(tmpAddrinfoPtr != NULL)       //顺序检验返回结果，寻找有效的地址
    {
        if((sockfd = socket(tmpAddrinfoPtr->ai_family,tmpAddrinfoPtr->ai_socktype,tmpAddrinfoPtr->ai_protocol)) < 0)
        {
            printf("socketfd create wrong");
            exit(-1);
        }
        if(connect(sockfd,tmpAddrinfoPtr->ai_addr,tmpAddrinfoPtr->ai_addrlen) < 0)
        {
            close(sockfd);
            tmpAddrinfoPtr = tmpAddrinfoPtr->ai_next;
        }
        else
        {
            break;
        }
    }
    if(tmpAddrinfoPtr == NULL)      //连接失败，退出
    {
        printf("can not connect to the host!");
        exit(-1);
    }

    freeaddrinfo(result);       //释放getaddrinfo函数内分配的空间

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
                    exit(0);
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
