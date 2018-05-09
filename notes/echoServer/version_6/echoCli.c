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

void *copyto(void * sockfdPtr);
const unsigned int MAXLEN = 100;

int main(int args,char **argv)
{
    int sockfd;
    struct addrinfo hints,*result = NULL;
    struct addrinfo *tmpAddrinfoPtr = NULL;
    int reValue;
    char inStr[MAXLEN];
    pthread_t tid;

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
    //从端口获得数据并发送到服务器，使用线程
    pthread_create(&tid,NULL,&copyto,(void *)&sockfd);
    
    //接收服务器的回射数据并显示到端口
    while(1)
    {
        reValue = read(sockfd,inStr,MAXLEN);
        if(reValue < 0)      
        {
            printf("Read from server failed!");
            exit(-1);
        }
        else if(reValue == 0)
        {
            printf("The server close\n");
            exit(0);
        }
        if(write(fileno(stdout),inStr,reValue) < 0)
        {
            printf("fputs error!");
            exit(-1);
        }
    }
}


void *copyto(void * sockfdPtr)
{
    unsigned int curLen;
    char buffer[MAXLEN];
    int n; 
    unsigned int sendNum = 0;
    int sockfd = *(int *)sockfdPtr; 

    while(fgets(buffer,MAXLEN,stdin))
    {
        sendNum = 0;

        curLen = strlen(buffer);
    
        while(sendNum < curLen) //make sure all data was written
        {
            n = write(sockfd,buffer + sendNum,curLen - sendNum);
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
    }
    shutdown(sockfd,SHUT_WR);   //向服务器发送FIN分节
    return NULL;
}
