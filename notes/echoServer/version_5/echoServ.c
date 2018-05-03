#include <sys/errno.h>  //errno
#include <stdlib.h>     //exit
#include <strings.h>    //bzero
#include <string.h>     //strlen
#include <arpa/inet.h>  //inet_pton
#include <sys/socket.h> //socket function,socket connect
#include <netinet/in.h> //seq bytes adjust,socket address
#include <stdio.h>      //printf
#include <unistd.h>     //fork,exec,write
#include <sys/wait.h>     //waitpid
#include <sys/select.h>   //select
#include <sys/un.h>     //sockaddr_un

const unsigned int MAXLEN = 100;
const int MAXCLIENT = 100;
const char *servPath = "/tmp/uIPC";

int str_serv(FILE *fd,int sockfd);

int main()
{
    int sockfd,connectfd;
    struct sockaddr_un servAddr;
    int n;
    int client[MAXCLIENT];      //store the sockfd of the established connection
    fd_set allSet;              //store the static read set of select
    fd_set readSet;             //store the parameter for select
    int i;                      
    int maxfd;

    //set server address
    bzero(&servAddr,sizeof(servAddr));
    servAddr.sun_family = AF_LOCAL;
    strncpy(servAddr.sun_path,servPath,sizeof(servAddr.sun_path)-1);

    //get socket num
    if((sockfd = socket(AF_LOCAL,SOCK_STREAM,0)) < 0)
    {
        printf("Socketfd wrong!");
        exit(-1);
    }

    unlink(servPath);       //如果地址以前绑定过，需要先解绑
    //bind server address
    if((bind(sockfd,(struct sockaddr *)&servAddr,SUN_LEN(&servAddr))) < 0)
    {
        printf("Bind failed!");
        exit(-1);
    }

    //initial the client,allSet,readSet
    for(i = 0;i < MAXCLIENT;i++)
      client[i] = -1;       //-1 stand for no client in this position

    FD_ZERO(&allSet);
    FD_ZERO(&readSet);
    maxfd = sockfd;         //initial the maxfd
    
    //set monitor sockfd
    if((n = listen(sockfd,20)) < 0)   //20 is the sum of waiting queue and accepted queue
    {
        printf("listen failed!");
        exit(-1);
    }

    FD_SET(sockfd,&allSet);      //monitor the listen socketfd 
//    signal(SIGCHLD, sigChildHandler); //we do not use process 

    while(1)
    {
        readSet = allSet;
        if((n = select(maxfd + 1,&readSet,NULL,NULL,NULL)) < 0)
        {
            if(errno == EINTR)
              continue;
        }
        
        if(FD_ISSET(sockfd,&readSet))
        {
            if((connectfd = accept(sockfd,NULL,NULL)) < 0)
            {
                if(errno == EINTR)
                    continue;
            }
            for(i = 0;i < MAXCLIENT;i++)
            {
                if(client[i] == -1)
                {
                    client[i] = connectfd;
                    if(connectfd > maxfd)
                        maxfd = connectfd;
                    FD_SET(connectfd,&allSet);
                    break;
                }
            }

            if(i == MAXCLIENT)
            {
              printf("To many clients, can not accept current one");
              continue;
            }

            
        }
        
        for(i = 0;i < MAXCLIENT;i++)    //dispose the request
        {
            if(client[i] == -1)
              continue;
            if(FD_ISSET(client[i],&readSet))
            {
               if(str_serv(stdout,client[i]) == -1)
               {
                   //read EOF,close this connectfd
                  close(client[i]);
                  FD_CLR(client[i],&allSet);
                  client[i] = -1;     //clear the sockfd
               }
            }
        }
    }
}

int str_serv(FILE *fd,int sockfd)  //the fd parameter seems useless
{
    int nreaded;
    char buffer[MAXLEN];

    nreaded = read(sockfd,buffer,MAXLEN);

    if(nreaded < 0)
    {
        printf("Server read wrong!");
        return 0;
    }

    if(nreaded == 0)
      return -1;
    write(sockfd,buffer,nreaded);
    return 0;
}

