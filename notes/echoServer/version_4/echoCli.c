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
#include <signal.h>     //signal
#include <sys/select.h> //select
#include <sys/time.h>   //struct timeval

typedef void Sigfun (int);

void str_cli(FILE *fp,int sockfd);
void AlarmDispose(int sig);

int main(int args,char **argv)
{
    int sockfd;
    struct addrinfo hints,*result = NULL;
    struct addrinfo *tmpAddrinfoPtr = NULL;
    int reValue;
    Sigfun * sAlarmFun; 
    int nsec = 20;


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

    sAlarmFun = signal(SIGHUP,AlarmDispose); //将ALarm处理函数和信号绑定

    tmpAddrinfoPtr = result;
    while(tmpAddrinfoPtr != NULL)       //顺序检验返回结果，寻找有效的地址
    {
        if((sockfd = socket(tmpAddrinfoPtr->ai_family,tmpAddrinfoPtr->ai_socktype,tmpAddrinfoPtr->ai_protocol)) < 0)
        {
            printf("socketfd create wrong");
            exit(-1);
        }
        
        if(alarm(nsec) != 0)      //version4，设置超时位20s
            printf("alarm was already set\n");

        if(connect(sockfd,tmpAddrinfoPtr->ai_addr,tmpAddrinfoPtr->ai_addrlen) < 0)
        {
            if(errno == EINTR)
            {
                printf("connect timeout:%d s!",nsec);
            }
            alarm(0);       //关闭时钟
            close(sockfd);
            tmpAddrinfoPtr = tmpAddrinfoPtr->ai_next;
        }
        else
        {
            alarm(0);
            break;
        }
    }
    signal(SIGALRM,sAlarmFun);          //还原信号处理函数
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
    fd_set rdSet;       //version4 用于给read函数设置超时
    struct timeval tVal;
    int seconds = 10;   //select的计时秒数 
    int wdSeconds = 8;  //setsockopt设置write超时的秒数

//设置套接字选项，所有该套接字的写都不能超多wdSeconds
    tVal.tv_sec = wdSeconds;
    tVal.tv_usec = 0;
    if(setsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,&tVal,sizeof(struct timeval)) < 0)
    {
        printf("Set socket write timeout option failed!");
    }
    
    while(fgets(inStr,MAXLEN,fp))
    {
        sendNum = 0;

        curLen = strlen(inStr);
    
        while(sendNum < curLen) //make sure all data was written
        {
            n = write(sockfd,inStr + sendNum,curLen - sendNum);
            if(n < 0)
            {
                if(errno == EINTR || errno == EWOULDBLOCK)
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
        
        //适用select监测套接字可读，如果超时不可读则报错
        FD_ZERO(&rdSet);    //设置select监测的集合
        FD_SET(sockfd,&rdSet);

        tVal.tv_sec = seconds;
        tVal.tv_usec = 0;
        n = select(sockfd + 1,&rdSet,NULL,NULL,&tVal);
        if(n == 0)
          printf("read from server:time out!");
        else
        {
            n = read(sockfd,inStr,MAXLEN);
            if(n <= 0)      //read after write can not be EOF
            {
                printf("Read from server failed!");
                exit(-1);
            }
        }
        if(fputs(inStr,stdout) == EOF)
        {
            printf("fputs error!");
            exit(-1);
        }

    }
}

//该函数不必处理任何事情，使用alarm信号的原因是，其超时会中断connect程序。
void AlarmDispose(int sig)
{
    return;
}
