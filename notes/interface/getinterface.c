#include <stdio.h>      //strcpy,printf
#include <sys/ioctl.h>  //function ioctl
#include <sys/socket.h> //socket functon:socket    
#include <net/if.h>     //struct ifconf and struct ifreq
#include <wchar.h>      //malloc
#include <stdlib.h>     //exit
#include <sys/errno.h>  //errno
#include <arpa/inet.h>  //inet_ntop

#define max(a,b) (a) > (b) ? (a):(b)

const int addrBufferLen = 100;
int main()
{
   int sockfd;
   struct ifconf ifc;
   int len,lastlen;
   char *buffer;
   char addrBuffer[addrBufferLen];
   char *ptr;
   struct ifreq * ifr;

   if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0) //创建一个套接字，用来调用ioctl，可以是TCP也可以是UDP
   {
       printf("Create Socket Wrong!");
       exit(-1);
   }

/*用于ioctl的SIOCGIFCONF参数在缓冲区不够大的时候会截断输出，而一些实现并不返回错误，所以需要记录上次返回的大小和扩大缓冲区后返回的大小作比较，当二者相等的时候才可以认为全部返回了;一些实现在缓冲区大小不足时会返回EINVAL错误*/
   lastlen = 0;
   len = 100 * sizeof(struct ifreq);
   while(1)
   {
       buffer = malloc(len);
       ifc.ifc_buf = buffer;
       ifc.ifc_len = len;
//SIOCGIFCONF参数下，将返回结构的地址结构
       if(ioctl(sockfd,SIOCGIFCONF,&ifc) < 0)
       {
           if(EINVAL != errno || lastlen != 0)
           {
               printf("Call inctl Wrong!");
               exit(-1);
           }
       }
       else
       {
           if(lastlen == ifc.ifc_len)
             break;
           lastlen = ifc.ifc_len;
       }
       len = len + 10 * sizeof(struct ifreq);
       free(buffer);

   }
   
   buffer = (char *)ifc.ifc_buf;
   ptr = buffer;
   while(ptr < buffer + ifc.ifc_len)
   {
       ifr = (struct ifreq *)ptr;
       //根据ifreq中存储的实际地址结构大小，确定一个ifreq所占的大小
#ifdef HAVE_SOCKADDR_SA_LEN
       len = max(sizeof(struct sockaddr),ifr->ifr_addr.sa_len); //对于在ifr_addr中定义了自身长度的实现，直接确定下个指针的位置
#else
       if(ifr->ifr_addr.sa_family == AF_INET6)
         len = sizeof(struct sockaddr_in6);
       else
         len = sizeof(struct sockaddr);
#endif  

       //这里出现问题，sizeof(ifreq)结果为40，而sizeof(ifr->ifr_name) + len结果为32,所以增加一个max做判断。
       ptr += max(sizeof(struct ifreq),sizeof(ifr->ifr_name) + len);

//如果系统支持SIOCGIFCONF返回AF_LINK地址族，则可以得到硬件地址和接口索引
#ifdef HAVE_SOCKADDR_DL_STRUCT
       if(ifr->ifr_addr.sa_family == AF_LINK)
       {
           sdl = (struct sockaddr_dl *)$ifr->ifr_addr；
           printf("%s : \nHaddr:%s\nInterface Index:%d\n",ifr->ifr_name,sdl->sdl_data + sdl->sdl_nlen,sdl->sdl_index);
           continue;
       }
#endif
       if(ifr->ifr_addr.sa_family == AF_INET)
       {
           printf("%s have IPv4 address!\n",ifr->ifr_name);
       }
       else if(ifr->ifr_addr.sa_family == AF_INET6)
       {
           printf("%s have IPv6 address!\n!",ifr->ifr_name);
       }
       else
           printf("%s address type is unkown\n",ifr->ifr_name);
   }
   
   free(ifc.ifc_buf);
}
