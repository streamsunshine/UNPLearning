#include <sys/socket.h> //socket function
#include <string.h>
#include <signal.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h> //struct icmp
#include <unistd.h>     //fork read write
#include <strings.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>

//宏定义
#define BUFEER_LEM 1000
#define SENDBUFFER_LEN 64
#define RECVBUFFER_LEN 1500
#define ADDRLEN 180

//函数声明
//由接收函数调用，用来计算传输时间
unsigned long getTime(struct timeval *curtime,struct timeval *oldtime);
//计算校验和
unsigned short checksum(unsigned short *data,int datalen);
//接收函数，一直等待接收返回的icmp报文
void recvFrame(void);
//alarm的信号处理函数，用来发送一个icmp帧
void sendFrame(int signo);
//由主机名获取IP地址
struct addrinfo *hostname2addr(const char *hostname,const char *service,int family,int socktype);
//错误处理
void err_quit(const char *info);


int optFlag = 0;
struct pingType
{
    struct sockaddr *addr;
    socklen_t addrlen;
    int sockfd;
    short pid;
    int seqNum;
}pingInfo;


int main(int argc,char *argv[])
{
    int n;
    char * addressPtr = NULL;
    char ch;
    struct addrinfo *ai;
    char tmpBuffer[100];

    opterr = 0;     //opterr为0时getopt的错误不输出到stderr
    while((ch = getopt(argc,argv,"v")) != -1)
    {
        switch (ch)
        {
            case 'v':
                optFlag++;
                break;
            default:
                err_quit("Wrong option!");
        }
    }
    addressPtr = argv[optind];
//  printf("%d\n%s\n",optFlag,addressPtr);

    //获取数据传输的信息，初始化pingInfo
    ai = hostname2addr(addressPtr,NULL,0,0);//这里没有析构注意

   if(ai->ai_family != AF_INET)
    err_quit("The ping can not support this address type!");
    
    if(inet_ntop(ai->ai_family,&(((struct sockaddr_in *)ai->ai_addr)->sin_addr),tmpBuffer,100) == NULL)
      err_quit("Address convert wrong");

    printf("The host is %s,its addr is %s\n",ai->ai_canonname ? ai->ai_canonname : addressPtr,tmpBuffer);

    if((n = socket(AF_INET,SOCK_RAW,IPPROTO_ICMP)) < 0)
      err_quit("create socket file descriptor is Wrong!");
    
    pingInfo.sockfd = n;
    pingInfo.addr = ai->ai_addr;
    pingInfo.addrlen = ai->ai_addrlen;
    pingInfo.pid = getpid() & 0xffff;
    pingInfo.seqNum = 1;

//释放getaddrinfo函数申请的空间
    freeaddrinfo(ai);

    signal(SIGALRM,sendFrame);
    alarm(1);

    recvFrame();
}

void recvFrame(void)
{
   char recvBuffer[RECVBUFFER_LEN];
   struct msghdr msg;
   struct iovec iov;
   struct ip * ipFrame;
   struct icmp *icmpFrame;
   int len;
   char addr[ADDRLEN];
   unsigned long time;
   struct timeval curtime;
   int size;

   size = 60 * 1024;
   setsockopt(pingInfo.sockfd,SOL_SOCKET,SO_RCVBUF,&size,sizeof(size));

   bzero(&msg,sizeof(struct msghdr));
   msg.msg_name = pingInfo.addr;
   msg.msg_namelen = pingInfo.addrlen;
   msg.msg_iov = &iov;
   msg.msg_iovlen = 1;

   iov.iov_base = recvBuffer;
   iov.iov_len = RECVBUFFER_LEN;
   while(1)
   {
//       printf("A");
 //      fflush(stdout);

       if(recvmsg(pingInfo.sockfd,&msg,0) < 0)
         err_quit("recv message failed!");

       ipFrame = (struct ip *)recvBuffer;

       len = ipFrame->ip_hl << 2;
       icmpFrame = (struct icmp *)(recvBuffer + len); //IP head len 以4个字节为单位
       if(icmpFrame->icmp_type != ICMP_ECHOREPLY || icmpFrame->icmp_id != pingInfo.pid)
        continue;

       if(inet_ntop(AF_INET,&(((struct sockaddr_in *)pingInfo.addr)->sin_addr),addr,ADDRLEN) == NULL)
         err_quit("Address convert wrong");

       gettimeofday(&curtime,NULL);
       time = getTime(&curtime,(struct timeval *)icmpFrame->icmp_data);

       printf("%d bytes from %s: icmp_seq = %d,ttl = %d,time = %ldms\n",SENDBUFFER_LEN,addr,icmpFrame->icmp_seq,ipFrame->ip_ttl,time);
   }

}

unsigned long getTime(struct timeval *curtime,struct timeval *oldtime)
{
    unsigned long result;

    result = (curtime->tv_sec - oldtime->tv_sec)*1000;
    result += (curtime->tv_usec - oldtime->tv_usec)/1000;

    return result;
}
struct addrinfo *hostname2addr(const char *hostname,const char *service,int family,int socktype)
{
    int n;
    struct addrinfo *result,hints;

    bzero(&hints,sizeof(struct addrinfo));
    hints.ai_flags =  AI_CANONNAME;
    hints.ai_family = family;
    hints.ai_socktype = socktype;
    if((n = getaddrinfo(hostname,service,&hints,&result)) != 0)
    {
        printf("%s\n",gai_strerror(n));
        err_quit("Get addrInfo Wrong");
    }
    return result;
}
void sendFrame(int signo)
{
    struct msghdr msg;
    struct iovec iov;
    struct icmp *icmpFrame;
    int datalen = SENDBUFFER_LEN - 8;
    char sendbuffer[SENDBUFFER_LEN];

    alarm(1);
    icmpFrame = (struct icmp *)sendbuffer;
    icmpFrame->icmp_type = ICMP_ECHO;
    icmpFrame->icmp_code = 0;
    icmpFrame->icmp_id = pingInfo.pid;
    icmpFrame->icmp_seq = pingInfo.seqNum++;
    memset(icmpFrame->icmp_data,0xa5,datalen);

    if(gettimeofday((struct timeval*)icmpFrame->icmp_data,NULL) < 0)
      err_quit("get time wrongly");

    icmpFrame->icmp_cksum = 0;
    icmpFrame->icmp_cksum = checksum((unsigned short *)sendbuffer,SENDBUFFER_LEN);

    bzero(&msg,sizeof(struct msghdr));
    msg.msg_name = pingInfo.addr;
    msg.msg_namelen = pingInfo.addrlen;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    iov.iov_base = sendbuffer;
    iov.iov_len = SENDBUFFER_LEN;

//    sendto(pingInfo.sockfd,sendbuffer,SENDBUFFER_LEN,0,pingInfo.addr,pingInfo.addrlen);
    if(sendmsg(pingInfo.sockfd,&msg,0) < 0)
    {
        printf("errno is %d\n",errno);
        err_quit("send msg wrong");
    }
   
  //  printf("send %d\n",pingInfo.seqNum);
  //  fflush(stdout);
}
void err_quit(const char *info)
{
    printf("%s",info);
    exit(-1);
}

unsigned short checksum(unsigned short *data,int datalen)
{
    unsigned int result = 0;
    int nleft = datalen;
    unsigned short answer = 0;

    while(nleft > 1)
    {
        result += *data++;
        nleft -= 2;
    }

    if(nleft == 1)
    {
        *(unsigned char *)(&answer) = *(unsigned char *)data;   //处理奇数字节的情况
        result += answer;
    }

    result = (result & 0xffff) + (result >> 16);
    result += (result>>16);

    answer = ~result;
    return (answer);
}
