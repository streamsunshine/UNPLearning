## 通用套接字结构
文件bits/socket.h,在sys/socket.h中定义的是osockaddr.
```c
typedef unsigned short int sa_family_t;
     struct sockaddr
  {
    __SOCKADDR_COMMON (sa_);	/* Common data: address family and length.  */
    char sa_data[14];		/* Address data.  */
  };
```  
其中的__SOCKADDR_COMMON的定义在/bits/sockaddr.h中，如下：
```c
#define	__SOCKADDR_COMMON(sa_prefix) \
  sa_family_t sa_prefix##family
```  
其中的##用来连接字符，对于sockaddr结构来说，其第一行为sa_family_t sa_family  
由于套接字函数需要处理所支持的任何协议族的套接字地址结构，所以需要定义一个通用的套接字地址结构，在调用套接字函数时，需要将套接字地址结构的引用强制转换为通用套接字结构的引用。
## IPv4的套接字地址结构
定义在/netinet/in.h
```c
typedef uint32_t in_addr_t;
typedef uint16_t in_port_t;
struct in_addr
{
    in_addr_t s_addr;
};
struct sockaddr_in
  {
    __SOCKADDR_COMMON (sin_);
    in_port_t sin_port;			/* Port number.  */
    struct in_addr sin_addr;		/* Internet address.  */

    /* Pad to size of `struct sockaddr'.  */
    unsigned char sin_zero[sizeof (struct sockaddr) -
			   __SOCKADDR_COMMON_SIZE -
			   sizeof (in_port_t) -
			   sizeof (struct in_addr)];
  };
```  
在使用套接字地址结构时，总是将其置0。套接字地址结构只在给定的主机上使用，并不在主机之间传递。  
固定大小16B

## IPv6的套接字地址结构
```c
struct in6_addr
  {
    union
      {
	uint8_t	__u6_addr8[16];
#if defined __USE_MISC || defined __USE_GNU
	uint16_t __u6_addr16[8];
	uint32_t __u6_addr32[4];
#endif
      } __in6_u;
    struct sockaddr_in6
  {
    __SOCKADDR_COMMON (sin6_);
    in_port_t sin6_port;	/* Transport layer port # */
    uint32_t sin6_flowinfo;	/* IPv6 flow information */
    struct in6_addr sin6_addr;	/* IPv6 address */
    uint32_t sin6_scope_id;	/* IPv6 scope-id */
  };
```
在IPv6地址结构中，如果sockaddr_in6是64位对其的，则sin6_addr也是64位对齐的。  固定大小28B

## 新的通用套接字地址结构  
在bits/socket.h文件中,定义如下：
```c
/* Structure large enough to hold any socket address (with the historical
   exception of AF_UNIX).  We reserve 128 bytes.  */
#define __ss_aligntype	unsigned long int
#define _SS_SIZE	128
#define _SS_PADSIZE	(_SS_SIZE - (2 * sizeof (__ss_aligntype)))

struct sockaddr_storage
  {
    __SOCKADDR_COMMON (ss_);	/* Address family, etc.  */
    __ss_aligntype __ss_align;	/* Force desired alignment.  */
    char __ss_padding[_SS_PADSIZE];
  };
```
新的通用套接字地址结构是足够大的。 
能够满足最苛刻的地址对其要求。  
除了ss_family其余部分对于用户是透明的。sockaddr_storage需要复制或者强制类型转换到其ss_family所指定的类型的套接字地址结构中，才能访问其他字段。
```c
example：
    struct sockaddr_storage cliaddr;
    // ...

    if(cliaddr.ss_family == AF_INET)
    {
            struct sockaddr_in *sockp = (struct sockaddr_in *) &cliaddr;
                // ...
    }
    else if(cliaddr.ss_family == AF_INET6)
    {
            struct sockaddr_in6 *sockp = (struct sockaddr_in6 *) &cliaddr;
                // ...
    }
```
