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
#include <signal.h>
#include <setjmp.h>

#define max(a,b) (a)>(b)?(a):(b)
#define MAX_RESEND_TIMES 3  //超时重发的次数
#define RTT_MIN 2       //计算得到的RTT的最大值和最小值
#define RTT_MAX 60

const int portNum = 7;
unsigned int SeqNum = 0;    //发送数据时的序列号，发送数据前增1
static sigjmp_buf jmpbuf;   //用来解决竞争问题的
unsigned int resendCount = 0;   //超时重发计数，超过最大次数退出程序

void str_cli(FILE *fp,int sockfd);

void Gettimeofday(struct timeval *tv);  //获取当前时间到tv
int gettime(void);      //由上一个函数的tv计算出距离1970.1.1的秒数
void updateRtt(int curtime,int sendtime);//成功接收后更新RTT
void sigAlarmHandler(int signo);
void rttInitial(void);  //在主函数中，程序运行之初，初始化RTT
void rtt_timeout(void); //RTT超时后的一些处理，不包括重发

int main(int args,char **argv)
{
    int sockfd;
    struct sockaddr_in servAddr;
    const char *defaultAddr = "127.0.0.1";
    if(args != 2)
    {
        printf("Use the default address 127.0.0.1\n");
    }
    else
    {
        defaultAddr = argv[1];
    }

    if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
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

    signal(SIGALRM,sigAlarmHandler);
    rttInitial();
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
    while(1)
    {
        if((n = read(fileno(fp),inStr,MAXLEN)) <= 0)
        {
            printf("process end!");
            return;
        }
        
        if((n = dgSendRecv(sockfd,inStr,n,wrStr,MAXLEN)) <= 0)
        {
            printf("read Wrong!");
            return;
        } 
        if((n = write(fileno(stdout),wrStr,n)) <= 0)
        {
            printf("read Wrong!");
            return;
        } 
    }
}

typedef unsigned int uint;
struct relHead  //为每一个UDP数据报增加的头
{
    uint ts;
    uint seq;
};

struct rttInfo  //存储rtt计算相关的数据
{
    float rttratio;
    float drttratio;
    float rtt;
    float drtt;
};

static struct rttInfo rttData;

int dgSendRecv(int sockfd,char *inStr,int n,char *wStr,int recvlen)
{
    struct msghdr sendHdr,recvHdr;
    struct iovec sendData[2],recvData[2];
    struct relHead sendRelHead,recvRelHead;
    struct timeval tv;
    int tmptime;
    
    resendCount = 0;
//设置发送信息结构，填充序列号，发送时间戳在发送前填入
    bzero(&sendHdr,sizeof(struct msghdr));
    sendHdr.msg_iov = sendData;
    sendHdr.msg_iovlen = 2;

    sendData[0].iov_base = &sendRelHead;
    sendData[0].iov_len = sizeof(struct relHead);

    sendData[1].iov_base = inStr;
    sendData[1].iov_len = n;

    sendRelHead.seq = ++SeqNum;
//设置接收结构,接收数据大小通过返回值返回
    bzero(&recvHdr,sizeof(struct msghdr));
    recvHdr.msg_iov = recvData;
    recvHdr.msg_iovlen = 2;
    recvData[0].iov_base =&recvRelHead;
    recvData[0].iov_len = sizeof(struct relHead);
    recvData[1].iov_base = wStr;
    recvData[1].iov_len = recvlen;

    //发送消息
    sendRelHead.ts = gettime();//获取当前时间（s）
    
    if(sendmsg(sockfd,&sendHdr,0) <= 0)
    {
        printf("send to serve failed");
        exit(-1);
    }

    alarm(rttData.rtt);
    while(1)
    {
        if(sigsetjmp(jmpbuf,1) != 0)
        {
            resendCount++;
            if(resendCount > MAX_RESEND_TIMES)
            {
                printf("Send failed!Check your connection|");
                exit(-1);
            }
            else
            { 
                sendRelHead.ts = gettime();
                sendRelHead.seq = ++SeqNum;

                if(sendmsg(sockfd,&sendHdr,0) < 0)
                {
                    printf("send to serve failed");
                    exit(-1);
                }
                alarm(rttData.rtt);
                continue;
            }
        }

        n = recvmsg(sockfd,&recvHdr,0);
        if(n < 0)
        {
            if(errno != EINTR)
            {
                printf("recive massage wrong!");
                exit(-1);
            }
        }
        if(n < sizeof(struct relHead) || recvRelHead.seq != SeqNum)  //序列号不正确，丢弃
          continue;
        alarm(0);       //接收到正确的消息，关闭时钟
        tmptime = gettime(); 
        updateRtt(tmptime,recvRelHead.ts);
        return n - sizeof(struct relHead);
    }
}

void Gettimeofday(struct timeval *tv)
{
    if(gettimeofday(tv,NULL) < 0)
    {
        printf("time get wrong!");
        exit(-1);
    }
}               

void updateRtt(int curtime,int sendtime)
{
    float sampletime = curtime - sendtime; 
    double dtime = sampletime - rttData.rtt;
    rttData.rtt = rttData.rtt + rttData.rttratio * dtime;

    if(dtime < 0)
      dtime = -dtime;

    rttData.drtt = rttData.drtt + rttData.drttratio * (dtime - rttData.drtt);
    rttData.rtt = rttData.rtt + 4 * rttData.drtt;

    if(rttData.rtt > RTT_MAX)
      rttData.rtt = RTT_MAX;
    if(rttData.rtt < RTT_MIN)
      rttData.rtt = RTT_MIN;
}

int getrtt(void)
{
    return (int)(rttData.rtt + 0.5);
}

void sigAlarmHandler(int signo)
{
    siglongjmp(jmpbuf,1);
}

void rttInitial(void)
{
    rttData.rtt = 2;
    rttData.drtt = 0;
    rttData.rttratio = 0.125;
    rttData.drttratio = 0.25;
}

int gettime(void)
{
    struct timeval tv;
    Gettimeofday(&tv);
    return (tv.tv_sec + tv.tv_usec/1000000); 
}

void rtt_timeout(void)
{
    printf("recive timeout! Resend\n");
    rttData.rtt *=2;    //超时则将rtt乘以2
    if(rttData.rtt > RTT_MAX)
      rttData.rtt = RTT_MAX;
    if(rttData.rtt < RTT_MIN)
      rttData.rtt = RTT_MIN;
}
