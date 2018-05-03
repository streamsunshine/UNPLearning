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
#include <netdb.h>      //getaddrinfo

#define max(a,b) (a)>(b)?(a):(b)


const int portNum = 80;
void str_cli(FILE *fp,int sockfd);
int Connect(int sockfd,const struct sockaddr *servaddr,socklen_t addrlen);

int main(int args,char **argv)
{
    int sockfd;
    struct addrinfo hints,*result;
    const char *hostname = "www.foobar.com";
    const char *dataReq = {};
    int n;

    if(args != 2)
    {
        printf("Connect to www.foobar.con\n");
    }
    else
    {
        hostname = argv[1];
    }

    bzero(&hints,sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    n = getaddrinfo(&hostname,&portname,&hints,&result);

    if(n != 0)
    {
        printf("%s",gai_strerror(errno));
        exit(-1);
    }

    tmpAddrPtr = result;
    while(tmpAddrPtr != NULL)
    {
        for(int i = 0;i < conTotal - conNum;i++)
        {
        if((sockfd = socket(tmpAddrPtr->ai_family,tmpAddrPtr->ai_socktype,0)) < 0)
        {
            break;
        }

        flags = fcntl(sockfd,F_GETFL,0);
        fcntl(sockfd,flags | O_NONBLOCK);

        if(connect(sockfd,(struct sockaddr *)tmpAddrPtr->ai_addr,tmpAddrPtr->ai_addrlen) < 0)
        {
            if(errno == EINPROGRESS)
            {
                FD_SET(sockfd,&checkSet);
                maxCheckfd = max(maxCheckfd,sockfd);
            }
            else
            {
                close(sockfd);
              break;
            }
        }
        else
        {
            conNum++;
            FD_SET(sockfd,&connectSet);
            maxConnectfd = max(maxConnectfd,sockfd);
        }
        }

        t.tv_sec = connectTimeoutS;
        t.tv_usec = connectTimeoutUs;

        count = 0;          //计算检测的套接字中，可读的有多少
        while(count < checkfdArraySize)
        {
            tmpFd = maxCheckfd;
            tmpT = t;
            n = select(maxCheckfd + 1,NULL,&tmpfd,NULL,&t);
            if(n <= 0)
              break;
            for(i = 0;i < checkfdArraySize;i++)
            {
                if(FD_ISSET(checkfdArray[i],tmpfd))
                {
                    len = sizeof(error);
                    n = getsockopt(checkfdArray[i],SOL_SOCKET,SO_ERROR,&error,&len);
                    if(n >= 0)
                    {
                        FD_SET(checkfdArray[i],connectSet);
                        maxConnectfd = max(maxConnectfd,checkfdArray[i]);
                        conNum ++;
                    }
                    else
                    {
                        close(checkfdArray[i];
                    }
                    count++;
                    FD_CLR(checkfdArray[i].&maxCheckfd);
                }
            }
        }
        if(conNum == 0)
          tmpAddrPtr = tmpAddrPtr.ai_next;
    }

    if(tmpAddrPtr == NULL)
    {
        printf("connect to server %s failed",hostname);
        exit(-1);
    }
    freeaddrinfo(result);
    
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


int Connect(int sockfd,const struct sockaddr *servaddr,socklen_t addrlen);
