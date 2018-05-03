## 系统函数

介绍套接字编程会用到的系统函数，包括读写，进程，线程等的操作函数（头文件为函数上面的第一个说明的头文件）

**头文件**:unistd.h

### read函数

用于从描述符（文件，套接字等）中读取数据。

`ssize_t read(int fd,void *buf,size_t count);`

**fd**:描述符。文件对应着open的返回值，套接字对应是socket的返回值。

**buf**:存放读取数据的缓冲区。 **count**：需要读取的数据大小。

#### 返回值

成功： 返回实际读取数据量的大小。遇到EOF返回0

失败： 返回-1，并设置相应的errno值

#### 注意

1.对于文件描述符，函数立即返回。对于套接字描述符函数一般设置为阻塞

### write函数

向打开的设备或文件中写数据

`ssize_t write(int fd,const void *buf,size_t count);`

参数说明和read类似

#### 返回值

成功： 返回写入的字节数

失败： 返回-1，并置errno的值

#### 注意

1.对于一般文件一般返回值等于count。对于其他文件则存在风险，因此要进行检查以保证所有数据写入到目标中。


### fork函数

用于创建一个和父进程相同的子进程。

`pid_t fork(void);`

#### 返回值

成功： 该函数一次调用有两个返回值。父进程中返回子进程的进程ID，子进程中返回0；

失败： 返回-1

#### 注意

子进程可以通过getppid()获取父进程的进程ID；父进程只能通过维护fork的返回值获得子进程ID。

### exec

替换当前进程中的内容（不改变进程ID）使之运行自己指定的程序。

`int execl(const char *pathname, const char *arg0,....,);`

`int execv(const char *pathname, char *const arg[]);`(arg数组的元素值是不可以改变的)

`int execle(const char *pathname, const char *arg0,...,(char *)0,char *const envp[]);` (char *)0:表示参数结束，后面的是环境变量

`int execve(const char *pathname, char *const argv[],char *const envp[]);`

`int execlp(const char *filename, const char *arg0,...,);`

`int execvp(const char *filename, char *const argv[]);`

#### 注释

l表示参数一个一个给出； v表示参数由参数数组给出； e表示有环境变量（对于由l参数的，参数结尾用(char *)0表示； p表示可执行文件名用文件名而非路径名表示

### fgets

`char *fgets(char *s,int size,FILE *stream);`

函数在读到换行符，达到size-1或者遇到一个EOF时结束.会读入这个换行符。遇到EOF函数
返回NULL不读入EOF。

**s**缓冲区

**size**读取最多size-1大小的字符最后一个要存放\0；对于读入换行符的情况可以通过s[strlen(s)-1]=’\0’;来处理。

#### 返回值

成功：返回字符数组的首地址
失败：即读到EOF，返回NULL

#### 注意

系统总会为读到的数据添加’\0‘需要处理的只有将换行符转换为’\0’的操作，以避免换行符影响

### fputs

`int fputs(const char *s,FILE *stream);`

向文件中写入字符串（不会写入\0）。写入后会使得文件位置指针后移。

**s**字符数组

**fp**文件指针

#### 返回值

成功:返回非负整数
失败:返回EOF（值为-1）

### waitpid

头文件：sys/wait.h;sys/types.h提供pid_t的定义

用于处理僵死进程。

`pid_t waitpid(pid_t pid,int *statloc,int options);`

**pid**:指定想要等待的进程ID，值-1表示等待的第一个终止的子进程。 

**statloc**:用来获取子进程的中止状态，可以用一些宏来判定。如果不关心，这位可以传入NULL。

**options**:指定一些附加的选项。最常见的是WNOHANG告知没有已终止内核时不要阻塞。

#### 返回值

成功：返回已终止进程的PID，如果使用了WNOHANG同时没有以退出的进程可收集，返回0；

失败：返回-1，原因存在errno中

### signal

头文件:signal.h

`typedef void(\*sigHandler)(int);`

`sigHandler signal(int sigNum,sigHandler handler);`

将信号处理函数绑定到信号上

**signum**:指定信号类型

**handler**:信号处理函数，也可以使用SIG_IGN表示忽略该信号，SIGDFL表示使用默认的处理

#### 返回值

成功：返回原信号处理函数指针（该函数指针可用于信号处理函数的恢复）。

失败：返回SIG_ERR（-1）

#### 注：

signal设置的信号处理函数在使用完，要恢复原来处理函数。

### sigprocmask

检测或更改进程的信号屏蔽字。对于多线程的进程，信号屏蔽字使用其他函数。

`int sigprocmask(int how,const sigset_t *restrict set,sigset_t *restrict oset);`

其中restrict，C语言中的一种类型限定符，用于告诉编译器，对象已经被指针所引用，不能通过除该指针外所有其他直接或间接的方式修改该对象的内容。

**how**：可以去三个值。1、SIG_BLOCK，新的信号屏蔽字是当前信号屏蔽字和set中指定的并集
2、SIG_UNBLOCK，set中包含希望解除阻塞的信号。新的信号屏蔽字是当前信号屏蔽字和 set指向信号集的补集的交集。
3、SIG_SETMASK，新的信号屏蔽是set指向的的值

**set**：如果set为空，则不改变当前信号屏蔽字

**oset**：非空时，当前进程屏蔽字通过oset返回。

#### 说明-有关信号集

定义信号集：sigset_t set;
```c
int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
int sigaddset(sigset_t *set,int signo);
int sigdelset(sigset_t *set,int signo);
int sigismember(sigset_t *set,int signo);
```
1、清空信号集。2、将所有信号填入信号集。3、添加信号。4、删除信号。5、判断是否为成员。

返回值：

前四个函数成功返回0；失败返回-1。最后一个函数，若真返回1，若假，返回0；

### alarm

头文件：unistd.h

`unsigned int alarm(unsigned int seconds);`

seconds:设置系统经过seconds秒后，发送SIGALRM给当前进程，其默认信号处理函数为中断进程。  当seconds为零时，取消定时。 

#### 返回值

上次计时未结束：返回上次计时的剩余时间，并覆盖上次计时（对于seconds为零也适用）

#### 说明

如果设置了信号处理函数，当前进程运行的函数会返回，并置errno=EINTR。  结合这一特性，alarm函数可以用于为connect，read，write等设置超时。

### select

头文件:sys/select.h sys/time.h

`int select(int maxfdp1,fd_set *readset,fd_set *writeset,fd_set *exceptset,const struct timeval *timeout);`

当readset中描述符可读，writeset中的可写，exceptset中的有错误是select函数返回

**maxfdp1**:三个描述符集合中，描述符最大值加1的值。告知内核检查到哪一个描述符。

**readset**：需要检查可读的描述符集合

**timeout**：如果select阻塞时间超过了timeout规定值，select函数返回

```c
struct timeval
{
    __time_t tv_sec;    //seconds
    __suseconds_t tv_usec;  //Microseconds
}
```

#### 返回值

情况1：如果有就绪的，返回就绪描述符个数

情况2：超时，返回0

情况3：出错，返回-1

#### 用法

参见 ../echoServer/version_2/echoCli.c

#### 注意

每次调用select函数，其readset，writeset，timeval等都会被改变，下次调用前需要重新设置。

### pselect

头文件：sys/select.h signal.h time.h

`int pselect(int maxfdp1,fd_set *readset,fd_set *writeset,fd_set *exceptset,const struct timespec *timeout,const sigset_t *sigmask);`

#### 参数

前部分参数参见select函数，区别体现在两个方面

1、其时间由timespec指定
```c
struct timespec
  {
    __time_t tv_sec;		/* Seconds.  */
    __syscall_slong_t tv_nsec;	/* Nanoseconds.  */
  };
```
其第二部分指定的纳秒。

2、pselect指定了sigmask，是一个指向信号掩码的指针，指示程序在pselect中禁止递交哪些信号。pselect函数中对于信号递交的控制只与sigmask有关。在调用pselect时，切换到sigmask，在pselect返回后，切换回原来的掩码设置。

这样可以允许程序先禁止递交某些信号，然后测试当前被禁止信号的处理函数设置的全局变量。这样可以避免，在检测过程中到来信号，造成信号丢失。而由于pselect可以设置自己的信号掩码，所以pselsect可以正常地在信号到来时被中断，从而避免阻塞在pselect中。

总结：通过这样的设置，解决了信号竞争问题。因为pselect参与下，设置信号掩码，测试描述符，恢复信号掩码，变成了一个原子操作。

### shutdown

头文件：sys/socket.h

`int shutdown(int sockfd,int howto);`

激发TCP终止序列，不管套接字的引用计数；可以指定关闭连接读这一半/关闭连接写这一半

**sockfd**:操作的套接字

**howto**：SHUT_RD关闭读那一半，SHUT_WR关闭写那一半，SHUT_RDWR都关闭。

#### 返回值

成功：返回0
失败：返回-1

### poll

头文件：poll.h

`int poll(struct pollfd *fdarray,unsigned long nfds,int timeout)`

功能类似于select

**fdarray**：每个需要监测的描述符对应一个结构体，将所有需要监测的描述符结构体组成一个数组

**nfds**：结构体数组中的元素个数

**timeout**：等待时长

#### 返回值

情况1：返回就绪的描述符个数

情况2：超时，返回0

情况3：出错，返回-1

### recvfrom

头文件：sys/socket.h

从UDP服务端套接字与未调用connect函数的UDP客户端套接字接收对端数据

`ssize_t recvfrom(int sockfd,void *buff,size_t nbytes,
            
int flags.struct sockaddr *from,socklen_t *addrlen)`

**sockfd**:套接字符号

**buff，nbytes**：存储接受信息的缓冲区及其大小

**flags**：暂默认为0

**from，addrlen**：数据源地址及其长度:

#### 返回值

成功：返回读到的字节数

失败：返回-1

### sendto

用于UDP服务端套接字与未调用connect函数的UDP客户端套接字。

`ssize_t sendto(int sockfd,const void *buff,size_t nbytes,
            
int flags,const struct sockaddr *to,socklen_t addrlen);`

#### 注释

参数和返回值参见recvfrom

### recvmsg

该函数集合了read，readv，recv。recvfrom的功能，是最全面的接收函数。

`ssize_t recvmsg(int sockfd,struct msghdr *msg,int flags);`

**flags**:可以参照unix网络编程14章的说明或者头文件。对于recvmsg函数，flags的值被复制到msghdr中的msg_flags中传递给内核；对于sendmsg函数flags直接传递给内核，msg_flags无效。

#### 返回值

成功：发送的字节数

失败：-1

#### 必要的结构体
```c
struct msghdr
  {
    void *msg_name;		/* Address to send to/receive from.  */
    socklen_t msg_namelen;	/* Length of address data.  */

    struct iovec *msg_iov;	/* Vector of data to send/receive into.  */
    size_t msg_iovlen;		/* Number of elements in the vector.  */

    void *msg_control;		/* Ancillary data (eg BSD filedesc passing). */
    size_t msg_controllen;	/* Ancillary data buffer length.
				   !! The type should be socklen_t but the
				   definition of the kernel is incompatible
				   with this.  */

    int msg_flags;		/* Flags on received message.  */
  };

/* Size of object which can be written atomically.

   This macro has different values in different kernel versions.  The
   latest versions of the kernel use 1024 and this is good choice.  Since
   the C library implementation of readv/writev is able to emulate the
   functionality even if the currently running kernel does not support
   this large value the readv/writev call will not fail because of this.  */
#define UIO_MAXIOV	1024

/* Structure for scatter/gather I/O.  */
struct iovec
  {
    void *iov_base;	/* Pointer to data.  */
    size_t iov_len;	/* Length of data.  */
  };

/* Structure used for storage of ancillary data object information.  */
struct cmsghdr
{
    size_t cmsg_len;		/* Length of data in cmsg_data plus length
				   of cmsghdr structure.
				   !! The type should be socklen_t but the
				   definition of the kernel is incompatible
				   with this.  */
    int cmsg_level;		/* Originating protocol.  */
    int cmsg_type;		/* Protocol specific type.  */
};

```
其中前两项用于非连接套接字，用来返回对端的地址。对于已连接套接字，msg_name应该为NULL。

iovec相关的两项用于多缓冲区操作。可以将接收到的数据存储到msg_iov数组指定的各个缓冲区中。按顺序存，存满一个缓冲区后，存下一个缓冲区。

msg_control两项是辅助数据(也称为控制信息)缓冲区的首地址和大小.辅助数据一般结合套接字选项，返回或者传递一些额外的数据。一个msg_control缓冲区可能用来返回多组数据，因此需要有一个结构定义单个数据，这个结构就是cmsghdr，其中存储着该组数组的信息。由于不同数据之间存在填充，因此定义了一些宏定义来处理存储在msg_control中的数据。宏定义定义在sys/param.h文件中。

msg_flags 只有recvmsg使用。用来将flags的值传递给内核，并将内核更新后的值返回。

#### 辅助数据的宏

```c
/* Ancillary data object manipulation macros.  */
//返回cmsg中数据的第一个字节的指针
unsigned char *CMSG_DATA(struct cmsghdr *cmsg);

//返回msghddr对应的辅助数据cmsg的下一个辅助数据的指针
struct cmsghdr *CMSG_NXTHDR(struct msghdr *mhdr, struct cmsghdr *cmsg);

//返回辅助数据的首指针
struct cmsghdr *CMSG_FIRSTHDR(struct msghdr *mhdr);

/*返回在cmsghdr中存储指定大小的data的条件下，它占据的总空间大小。用来为辅助对象分配缓冲空间*/

unsigned int  CMSG_SPACE(unsigned int length);length为待存储data的大小
 
```

### sendmsg

可以实现send，sendto，write，writev的功能，是功能最全面的发送函数。

`ssize_t sendmsg(int sockfd,struct msghdr *msg,int flags);`

#### 注

参数解析参见recvmsg

### getaddrinfo

头文件：netdb.h

有主机名和服务名，获得与协议类型无关的地址信息

int getaddrinfo(const char *hostname,const char *service,const struct addrinfo *hints,struct addrinfo **result);

**hostname,service**:主机名称，服务名称

**struct addrinfo**：定义在netdb.h中info

struct addrinfo
{
  int ai_flags;			/* Input flags.  */
  int ai_family;		/* Protocol family for socket.AF_xxx  like AF_INET,还可以指定AF_UNSPEC，表示可以返回IPv4和IPv6*/
  int ai_socktype;		/* Socket type. SOCK_xxx like SOCK_STREAM */
  int ai_protocol;		/* Protocol for socket.  */
  socklen_t ai_addrlen;		/* Length of socket address.  */
  struct sockaddr *ai_addr;	/* Socket address for socket.  */
  char *ai_canonname;		/* Canonical name for service location.  */
  struct addrinfo *ai_next;	/* Pointer to next in list.  */
};

**hints**:可以设置ai_flags/family/socktype/protocol;用来控制函数的返回值;hint可以为空指针。

|ai_flags|说明|
|----|-----|
|AI_PASSIVE|套接字用于被动打开（服务器）|
|AI_CANONAM|返回主机的规范名字|
|AI_NUMERICHOST/SERV|hostname,service必须是数字形式的|
|AI_V4MAPPED|无IPv6时，使用IPv4的映射地址|
|AI_ADDRCONFIG|按照所在主机配置选择返回地址类型|

**result**:返回结果，是一个函数内部动态分配的一个链表，使用完需要由用户释放。

#### 返回值

成功：返回0；

失败：返回错误码，使用gai_strerror()获取错误信息。

### gai_strerror

const char *gai_strerror(int error);

**error**:getaddrinfo的非零返回值

#### 返回值

错误信息的字符串。

#### freeaddrinfo

void freeaddrinfo(struct addrinfo *ai);

释放getaddrinfo函数内部动态分配的空间。

### getnameinfo

`int getnameinfo(const struct sockaddr *sockaddr,socklen_t addrlen,char *host,socklen_t hostlen,char *serv,socklen_t servlen,int flags);`

注：

sockaddr一般由accept，recvfrom，getsockname。getpeername等返回。

hostlen，servlen为零，表示对对应的信息不感兴趣。

flags，用于控制函数的行为。

#### 返回值

成功：返回0

失败：返回非0

### getsockopt和setsockopt

头文件：sys/socket.h

用来获取当前套接字选项；设置套接字选项

`int getsockopt(int sockfd,int level,int optname,void *optval,socklen_t *optlen);`

`int setsockopt(int sockfd,int level,int optname,void *optval,socklen_t optlen);`

#### 参数

**sockfd**:一个已经打开的套接字

**level**:指明optname属于哪一类型。参见unix网络编程第七章或上网搜索。

**optname**:指明要设置的套接字选项名称。

**optval&optlen**:指定存储套接字选项返回值或设置值的buffer。

#### 返回值

成功：返回0

失败：返回-1

### fcntl

用于改变已经打开文件的属性

头文件：unistd.h   fcntl.h

`int fcntl(int fd,int cmd,long arg);`

**fd**：文件描述符

**cmd**：功能指令。

F_DUPFD：返回一个大于等于arg的未使用的最小描述符。新描述符和fd共享一个文件表项，但有自己的文件描述符标志，其FD_CLOEXEC被清除。

F_DUPFD_CLOEXEC：设置FD_CLOEXEC标志。（该标志设置表示该描述符在exec时，无效）

F_SETFD/F_GETFD：设置文件描述符标志为arg；函数返回值为文件描述符标志。目前只有FD_CLOEXEC一个文件描述符标志。

F_SETFL/F_GETFL：设置文件状态标志为arg；返回文件状态标志。

注：尚有其他命令暂不介绍

#### 返回值

成功：按照cmd指定的返回

失败：返回-1

### ioctl

网络编程中用于获取接口信息（如：接口地址，是否接受广播，是否支持多播），访问路由表，访问ARP，套接字操作，文件操作（设置清除阻塞标志）.他的有些操作可以使用fcntl实现。同一个操作也可能通过指定不同的request（函数参数）实现。

头文件：sys/ioctl.h

`int ioctl(int fd,int request,arg);`

**fd**:描述符,可以是UDP或者TCP的描述符

**request**：使用宏定义指定ioctl完成的功能。

**arg**：是指针，但是指针的类型取决于request参数。

#### 返回值

成功：返回0

失败：返回-1

#### 说明

1、访问接口

有关的参数由很多，在使用时查表即可。网络编程中常用的是获取主机的接口信息的函数，这里在notes/interface文件夹下，练习写一个相关的函数。以说明ioctl函数及其参数的使用。

这里使用的SIOCGIFCONF参数是获取很多其他参数的基础。这个参数能让ioctl返回当前主机所有接口的名称和地址列表，想获取接口的其他参数时，将ioctl返回的ifreq结构体复制一份传递到ioctl函数中。

结构总是以名字标识

#### 需要用到的结构体定义

struct ifconf
  {
    int	ifc_len;			/* Size of buffer.  */
    union
      {
	__caddr_t ifcu_buf;
	struct ifreq *ifcu_req;
      } ifc_ifcu;
  };
# define ifc_buf	ifc_ifcu.ifcu_buf	/* Buffer address.  */
# define ifc_req	ifc_ifcu.ifcu_req	

struct ifreq
  {
# define IFHWADDRLEN	6
# define IFNAMSIZ	IF_NAMESIZE
    union
      {
	char ifrn_name[IFNAMSIZ];	/* Interface name, e.g. "en0".  */
      } ifr_ifrn;

    union
      {
	struct sockaddr ifru_addr;
	struct sockaddr ifru_dstaddr;
	struct sockaddr ifru_broadaddr;
	struct sockaddr ifru_netmask;
	struct sockaddr ifru_hwaddr;
	short int ifru_flags;
	int ifru_ivalue;
	int ifru_mtu;
	struct ifmap ifru_map;
	char ifru_slave[IFNAMSIZ];	/* Just fits the size */
	char ifru_newname[IFNAMSIZ];
	__caddr_t ifru_data;
      } ifr_ifru;
  };
# define ifr_name	ifr_ifrn.ifrn_name	/* interface name 	*/
# define ifr_hwaddr	ifr_ifru.ifru_hwaddr	/* MAC address 		*/
# define ifr_addr	ifr_ifru.ifru_addr	/* address		*/
# define ifr_dstaddr	ifr_ifru.ifru_dstaddr	/* other end of p-p lnk	*/
# define ifr_broadaddr	ifr_ifru.ifru_broadaddr	/* broadcast address	*/
# define ifr_netmask	ifr_ifru.ifru_netmask	/* interface net mask	*/
# define ifr_flags	ifr_ifru.ifru_flags	/* flags		*/
# define ifr_metric	ifr_ifru.ifru_ivalue	/* metric		*/
# define ifr_mtu	ifr_ifru.ifru_mtu	/* mtu			*/
# define ifr_map	ifr_ifru.ifru_map	/* device map		*/
# define ifr_slave	ifr_ifru.ifru_slave	/* slave device		*/
# define ifr_data	ifr_ifru.ifru_data	/* for use by interface	*/
# define ifr_ifindex	ifr_ifru.ifru_ivalue    /* interface index      */
# define ifr_bandwidth	ifr_ifru.ifru_ivalue	/* link bandwidth	*/
# define ifr_qlen	ifr_ifru.ifru_ivalue	/* queue length		*/
# define ifr_newname	ifr_ifru.ifru_newname	/* New name		*/

2、访问ARP

ARP实现了IPv4地址到硬件地址的转换（IPv6不使用ARP）它维护了一个ARP表。ioctl有三个请求和ARP表有关，通过在结构体arpreq（头文件net/if_arp.h）指定网际网地址指定一个表项，可以实现添加修改，获取，删除三种操作。

3、路由表操作

使用头文件net/route.h中的rtentry结构体，可以进行路由表项的添加和删除。

### sysctl

头文件：sys/param.h sys/sysctl.h

用于获取各种系统信息，如文件系统，虚拟内存，内核限制，硬件，网络子系统等。对于网络编程，它可以倾泻接口列表，路由表，ARP高速缓存。

`int sysctl(int *name,u_int namelen,void *oldp,size_t *oldlenp,void *newp,size_t newlen);`

**name**：按等级说明的请求信息的类型，namelen为长度

**oldp**：内核返回信息存储的缓冲区，oldlenp返回信息的大小。缓冲区不够大，返回ENOMEM

**newp**：设置新值时使用。不指定新值时，newp为NULL，newlen为0.

#### 说明

对于网络子系统，name的第一个参数为CTL_NET，第二个参数指定要获取或设置那种信息。第三个参数为协议，第四个参数为地址族，第五个选项为操作类型，第六个参数说明返回信息应该具有的标志。

不同的请求和操作内核返回的信息不同，要对应处理

### 接口名字和索引

分别用于接口名字转换为索引，索引转换为名字，获取接口名字和索引的列表，释放上一个函数的空间。

`unsigned int if_nametoindex (const char *__ifname);`

`char *if_indextoname (unsigned int __ifindex, char *__ifname);`
(ifname为缓冲区，大小为IFNAMSIZ)

`struct if_nameindex *if_nameindex (void);`

`void if_freenameindex (struct if_nameindex *__ptr);`

#### 返回值

失败返回0；失败返回NULL；

## 时间获取函数

### gettimeofday

头文件：sys/time.h unistd.h

获取当前时间和时区信息

`int gettimeofday(struct timeval *tv,struct timezone *tz);`

**tv**:时间信息。从1970年1月1日到现在的时间
```c
struct timeval   
{
    time_t       tv_sec;  /* seconds */
    suseconds_t  tv_usec; /* microseconds */
};
```
**tz**:时区信息。不感兴趣可以传入NULL
```c
struct timezone   
{   
        int tv_minuteswest;   /* 格林威治时间往西方的时差 */  
            int tv_dsttime;       /* 时间的修正方式 */  
};  
```
#### 返回值

成功:返回0

失败:返回-1,错误存在errno中;EFAULT辨识tv或tz所指向的存储空间超出存取权限.
