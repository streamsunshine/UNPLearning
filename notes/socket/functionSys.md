# 系统函数

## 读写函数

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



### fork函数

用于创建一个和父进程相同的子进程。

`pid_t fork(void);`

#### 返回值

成功： 该函数一次调用有两个返回值。父进程中返回子进程的进程ID，子进程中返回0；

失败： 返回-1

#### 注意

子进程可以通过getppid()获取父进程的进程ID；父进程只能通过维护fork的返回值获得子进程ID。

### 获取进程表示的程序

pid_t getpid(void); 获取自己的进程ID

pid_t getppid(void); 获取父进程的进程ID

uid_t getuid(void); 获取进程的实际用户ID

uid_t geteuid(void); 获取进程的有效用户ID

gid_t getgid(void); 获取进程的实际组ID

gid_t getegid(void); 获取进程的有效组ID

#### 说明

实际用户/组ID标识我们是谁，即调用程序的用户/组

有效用户/组ID决定我们程序对文件的访问权限。

有效用户ID使用的例子如passwd程序，在用户修改自己的密码时，由于/etc/passwd文件所有者为root，正常普通用户无法访问，但是通过对程序设置用户ID位，可以在程序运行时有效用户为其所有者，此时程序拥有其所有者的权限，如passwd就有了root的权限。从而能够访问/etc/passwd。

#### 修改实际/有效 用户/组 ID的函数

int setuid(uid_t uid);

int setgid(gid_t gid);

主要用来修改有效用户ID，在不需要特权的时候修改为实际用户ID。


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


### exec

头文件：unistd.h

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

### getopt

头文件：unistd.h

用来分析命令行参数。分析其中在调用程序时指定的额外的选项比如grep的-i 这种选项。

`int getopt(int argc,char *const argv[],const char* optstring);`

**argc,argv**:就是传递给main函数的那个。

**optstring**:程序的支持的选项。如"iv"表示程序支持两种选项-i和-v。对于选项后还要接参数的则在字符后加:，如"i:v"，表示-i后面可以接参数，-i write和-iwrite都对，但是-i不对。后面参数可选时使用::,如"i::"，-i和-iwrite都对，但是-i write不对。当出现不匹配时，函数返回？。

#### 返回值

正常：返回选项字符

当出现上面说的不匹配时，返回?

当getopt检测了所有argv中的内容时，返回-1.

#### 说明

函数使用几个全局变量存储一些额外的参数。

extern int opterr;opterr设置为0时，产生的错误不输出到stderr上。否则输出。

extern char *oparg;指向当前选项的参数，如上面的write。

extern int optind; 再次调用getopt时的下一个argv指针的索引。

extern int optopt; 最后一个位置选项。

## 信号相关的函数

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

### kill

向进程或进程组发送信号。

`int kill(pid_t pid,int signo);`

**pid**:pid > 0发送信号signo到pid对应的进程;pid = 0,发送给同进程组的所有进程；pid < 0,发送信号到进程组ID为pid绝对值的进程组中的所有进程。pid = -1，发送给所有当前进程有权限的进程。

注意：上述的所有进程，必须是当前进程有权限发送信号的，同时不包括内核进程和init这些位于系统进程集中的进程。


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

## 进程间通信

### 共享内存

共享内存是最快IPC方式，因为它能够减少进程和内核之间拷贝数据的次数。

由于子进程会拷贝父进程的全部副本，所以子进程的修改不会影响到父进程的值，所以需要使用mmap函数，声明共享内存。

### mmap



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

### gai_strerror

const char *gai_strerror(int error);

**error**:getaddrinfo的非零返回值

#### 返回值

错误信息的字符串。


## DNS名字地址转换

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


## 设置套接字选项

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

F_SETFL/F_GETFL：设置文件状态标志为arg；返回文件状态标志。文件状态标志有读、写，执行，阻塞等。

F_GETLK,F_SETLK,F_SETLKW：获取是否有锁，有锁为锁的情况，无是指l_type为F_UNLCK;设置锁，如果有锁则直接退出，并返回错误；设置锁，如果有锁则阻塞。其对应的arg为指向flock结构的指针。

#### flock结构
```c
struct flock 
{
    short l_type;/*F_RDLCK, F_WRLCK, or F_UNLCK*/
    off_t l_start;/*相对于l_whence的偏移值，字节为单位*/
    short l_whence;/*从哪里开始：SEEK_SET, SEEK_CUR, or SEEK_END*/
    off_t l_len;/*长度, 字节为单位; 0 意味着缩到文件结尾*/
    pid_t l_pid;/*returned with F_GETLK*/
};
```
锁类型： F_RDLCK(读共享锁), F_WRLCK(写互斥锁),和F_UNLCK(对一个区域解锁)

锁开始： 锁位置(l_whence),相对于l_whence要锁或者解锁的区域开始位置(l_start)

锁长度： 要锁的长度，字节计数(l_len)
        锁拥有者：记录锁的拥有进程ID，这个进程可以阻塞当前进程，仅F_GETLK形式返回

#### 说明
对于锁区域要注意的几点

1、锁可以开始或者超过文件当前结束位置，但是不可以开始或者超过文件的开始位置

2、如果l_len为0，意味着锁的区域为可以到达的最大文件偏移位置。这个类型，可以让我们锁住一个文件的任意开始位置，结束的区域可以到达任意的文件结尾，并且以append方式追加文件时，也会同样上锁。

3、如果要锁住整个文件，设置l_start 和 l_whence为文件的开始位置（l_start为0 l_whence 为 SEEK_SET ），并且l_len为0。

4、如果有多个读共享锁(l_type of F_RDLCK)，其他的读共享锁可以接受，但是写互斥锁(type ofF_WRLCK)拒绝

5、如果有一个写互斥锁(type ofF_WRLCK),其他的读共享锁(l_type of F_RDLCK)拒绝，其他的写互斥锁拒绝。

6、如果要取得读锁，这个文件描述符必须被打开可以去读；如果要或者写锁，这个文件的描述符必须可以被打开可以去写。

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

## 程序运行时间统计

linux中存在几种时间计数如下：

时钟时间（墙上时钟时间） ＝ 处于阻塞状态时间 ＋ 处于就绪状态时间 ＋处于运行状态时间

用户CPU时间 ＝ 运行状态下用户空间的时间

系统CPU时间 =  运行状态下系统空间的时间。

用户CPU时间+系统CPU时间=运行时间，也就是进程时间（CPU时间）。

程序处于阻塞状态时，cpu时间是不计数的。与这几种时间计算相关的函数有：

### times

头文件：sys/time.h

buf中返回用户和系统cpu时间，获取时钟时间需要两次调用times的返回值的时间差。

`clock_t times(struct tms *buf);`

#### 返回值

成功：返回相对于过去某个时刻的相对时钟时间，所以需要做差获得时钟时间

失败：返回-1

#### 相关结构体

```c
struct tms
  {
    clock_t tms_utime;		/* User CPU time.  */
    clock_t tms_stime;		/* System CPU time.  */

    clock_t tms_cutime;		/* User CPU time of dead children.  */
    clock_t tms_cstime;		/* System CPU time of dead children.  */
  };
```
clock_t 一般定义为long

### getrusage

头文件：sys/time.h

该函数可以程序统计对系统资源的使用量。（实际上是统计了进程执行直到调用该函数时的资源用量，如果在不同的时间调用该函数，会得到不同的结果）

`int getrusage(int who, struct rusage *usage);`

**who**:指定需要返回的系统资源用量，可以取以下值： 

RUSAGE_SELF：返回调用该函数进程的资源用量统计，会返回该进程下所有线程的资源用量之和。 

RUSAGE_CHILDREN：返回调用该函数进程所有已终止且被回收子进程的资源用量统计。如果进程有孙子进程或更远的后代进程，且这些后代进程和这些后代进程与调用进程之间的中间进程也已终止且被回收，那么这些后代进程的资源用量也会被统计。 

RUSAGE_THREAD（Linux 2.6.26起支持）：返回调用该函数线程的资源用量统计。 

#### 返回值

成功：返回0

失败：返回-1，同时设置errno的值。

#### 错误
EFAULT：usage指针指向不可访问地址。 
EINVAL：who被指定为无效值。 

#### 用到的结构体
```c
struct rusage {
    struct timeval ru_utime; /* user CPU time used */
    struct timeval ru_stime; /* system CPU time used */
    long   ru_maxrss;        /* maximum resident set size */
    long   ru_ixrss;         /* integral shared memory size */
    long   ru_idrss;         /* integral unshared data size */
    long   ru_isrss;         /* integral unshared stack size */
    long   ru_minflt;        /* page reclaims (soft page faults) */
    long   ru_majflt;        /* page faults (hard page faults) */
    long   ru_nswap;         /* swaps */
    long   ru_inblock;       /* block input operations */
    long   ru_oublock;       /* block output operations */
    long   ru_msgsnd;        /* IPC messages sent */
    long   ru_msgrcv;        /* IPC messages received */
    long   ru_nsignals;      /* signals received */
    long   ru_nvcsw;         /* voluntary context switches */
    long   ru_nivcsw;        /* involuntary context switches */
};
```

## 线程

### 线程对于信号的处理

每个线程可以使用`int pthread_sigmask(int how,const sigset_t *restrict set,sigset_t *restrict oset);`设置自己的信号屏蔽字。设置完后相应的信号不会导致线程进入信号处理程序。

线程可以调用`int sigwait(const sigset_t *restrict set,int *restrict signop);`等待set集合中的信号，在调用该函数前应该阻塞对应的信号。

虽然线程有独立的信号屏蔽字，但是所有线程共享信号处理函数。进程会将信号发送到每一个线程，除了与硬件故障有关的它会直接发送到引发这个事件的线程中去。

kill用于发送信号到进程，`int pthread_kill(pthread_t thread,int signo);`用于发送信号到特定的线程。

### 有关线程属性，同步原语，同步原语属性的init和destroy的使用

对于静态分配的同步原语，一般都是全局性的，生命期包含整个程序运行期间，所以不会destroy。而对于动态分配的同步原语，一般其生命期都不包含整个程序生命期，需要在一定时刻释放，所以需要调用destroy。而对于线程属性和同步原语属性，它们在初始化后就不在有用，所以即使是静态声明的，也需要调用destroy释放空间。

### 线程与fork

多线程的进程，在调用fork的时候，fork只复制当前调用fork函数的线程，如果其他线程中占有锁将对程序状态造成影响。所以需要进行处理。

`int pthread_atfork(void (*preapare)(void),void (*parent)(void),void (*child)(void));`

prepare在创建子进程前获取所有的锁，然后创建子进程。此时子进程和父进程都分别占有所有的锁，父进程调用parent函数解锁，子进程调用啊child函数解锁。

### pthread_create

头文件：pthread.h

创建线程

`int pthread_create(pthread_t *tid,const pthread_attr_t *attr,void *(*func)(void *),void *arg);`

**tid**:用来返回线程ID

**attr**:设置线程属性，可以设置线程是detach的还是join的，控制线程栈的大小/位置等与线程栈相关的属性。线程属性是一种类型的变量，有函数对其进行处理。使用默认属性则传入NULL。

**func**:线程执行的函数。其形参对应着arg，返回值将在pthread_join函数中返回。

**arg**:对应func函数的形参

#### 返回值

成功：返回0

失败：返回正的错误值

### pthread_join

等待给定线程终止（阻塞）

`int pthread_join(pthreat_t tid,void **status);`

**tid**:等待线程的ID

**status**:用于获取指定线程函数的返回值,若不需要置NULL

#### 返回值

成功:返回0
失败:返回正的错误值

### pthread_cancel

等待给定线程终止（阻塞）

`int pthread_cancel(pthreat_t tid);`

**tid**:要取消的线程的ID，取消是指不等待线程终止，向线程发送要求进程终止请求，不过线程可以选择忽略或者控制如何被取消。

#### 返回值

成功:返回0
失败:返回正的错误值

### 与线程取消相关的其他函数

设置线程是否可被取消`int pthread_setcancelstate(int state,int *oldstate);`函数为原子操作

设置线程取消检测点：`void pthread_testcancel(void);`线程取消，需要在特定的取消检测点进行，处理posix规定的，还可以自己设置。

设置取消的类型：`int pthread_setcanceltype(int type,int *oldtype);`上面说的是“推迟取消”，还有“异步取消”不必等到取消点。


### pthread_cleanup_push

设置线程退出时，其调用的函数。线程退出时调用的函数称为线程清理处理程序。

`void pthread_cleanup_push(void (*rtn)(void *),void *arg)`

**rtn**：为线程退出时，想要调用的函数

**arg**：为传递给函数rtn的参数

#### 说明

其中设置的函数，只在线程1、自身调用pthread_exit()退出时。2、其他线程调用pthread_cancel取消线程时。3、如果上面两个没有使用，则在pthread_clean_up_pop的参数非零的时候，会调用清理处理程序。

调用线程清理处理程序的顺序，和该函数设置的顺序相反。

### pthread_cleanup_pop

`void pthread_cleanup_pop(int execute)`

**execute**:当线程运行该函数时，如果execute不为零，则运行线程清理处理程序，如果为零，则不运行，但是同样会删除push建立的线程清理处理程序。

### pthread_self

获取自己的线程ID

`pthread_t pthread_self(void);`

#### 返回值

自己的ID

### pthread_detach

将线程转变为脱离状态

`int pthread_detach(pthread_t tid);`

**tid**:需要设置为脱离状态的线程。

#### 返回值

成功：返回0

失败：返回正的错误值

####  常见操作

线程将自己设置为脱离的：pthread_detach(pthread_self());

### pthread_exit

终止线程

`void pthread_exit(void *status);`

**status**:返回线程终止状态，不能是线程函数的局部变量，否则会随着线程一起消失。

线程也可以通过其他方式终止。1、启动线程的函数返回。其返回值就是线程的终止状态。2、进程main函数返回，或者任何线程调用了exit，整个进程将终止，此时该进程的所有线程也随之终止。


### 线程特定数据函数

### pthread_once

用于控制在多线程程序中，其指定的函数只在第一个调用的线程中运行。其指定的函数不单独调用，只和他列在一起就可以。

`int pthread_once(pthread_once_t *onceptr,void (\*init) (void));`

**onceptr**:once = PTHREAD_ONCE_INIT;onceptr = &once;其中的宏是一个固定值。once一般定义为一个全局变量。

**init**:运行一次的函数指针，如果函数需要参数，可以用一个不需要的参数封装。对于线程特定数据应用运行一次的函数为pthread_key_create().这个函数就需要封装。

### pthread_key_create

用于获取进程，线程中维护特定数据数组 的下标。

`int pthread_key_create(pthread_key_t *keyptr,void (\*destructor)(void *value));`

**keyptr**:用于返回数组下标。一般其对应的变量定义为全局变量，该变量应该被所有线程共享，且所有线程只读该变量，所以不存在线程不安全的问题。

**destructor**:析构函数。用来释放数组在键值处，存储的数据地址指向的数据。

#### 返回值

以上两个函数返回值相同

成功：返回0

失败：返回正的错误值

### pthread_key_delete

取消键与线程特定数据值之间的关联关系。但是不会激活与键关联的析构函数。

`int  pthread_key_delete(pthread_key_t key);`

### 返回值

同上

### pthread_getspecific

获取线程自身维护数组中存储的指针值，传入参数为pthread_key_create()返回的下标

`void *pthread_getspecific(pthread_key_t key);`

#### 返回值

见函数功能说明

### pthread_setspecific

设置线程自身维护数组中的指针值

`int pthread_setspecific(pthread_key_t key,const void *value);`

**key**:下标

**value**:设置的数组元素值

#### 返回值

成功：0

失败：正的错误值

#### 说明

value一般指向一段动态分配的内存区

### 互斥锁

互斥锁变量应该在主线程中定义和初始化。互斥锁在一定程度上回增加系统开销，但是并不大。

判断是否使用互斥锁的一种方法就是线程是否需要访问同一个全局变量或静态变量。

加锁的位置也是根据访问全局变量或者静态变量的情况.

使用多个互斥锁相互嵌套时，防止死锁发生。1、将资源访问排序，即对互斥锁访问排序。2、在上锁前，使用pthread_mutex_trylock测试，如果不能获得锁，就先释放所有已获得锁。如果能够获得则继续运行。

### pthread_mutex_init

用来初始化互斥量

`int pthread_mutex_init(pthread_mutex_t *restrict mutex,const pthread_mutexattr_t *restrict attr);`

**mutex**:需要初始化的互斥量的指针，可以是静态声明的变量取地址得到的，也可以是使用malloc动态分配的。对于动态分配的互斥量，在free之前，需要调用pthread_mutex_destroy。对于静态则不需要调用destroy函数。静态分配是指不用malloc分配的。

**attr**:互斥量的属性，如果设置为NULL，则使用默认的进行初始化。

#### 返回值

成功：0

失败：正的错误值

### pthread_mutex_destroy

用来destroy互斥量（析构）

`int pthread_mutex_destroy(pthread_mutex_t *mutex);`

**mutex**:需要destroy的互斥量的指针

#### 返回值

成功：0

失败：正的错误值

### pthread_mutex_lock

用来加锁，在线程解锁前，其间的代码和数据不能被另一个线程访问。

`int pthread_mutex_lock(pthread_mutex_t *mptr);`

**mptr**:互斥锁变量的指针。互斥锁变量应该使用PTHREAD_MUTEX_INITIALIZE这个宏变量进行初始化。

#### 返回值

成功：0

失败：正的错误值

### pthread_mutex_unlock

`int pthread_mutex_unlock(pthread_mutex_t *mptr);`

用于解锁，其余同上。

### pthread_mutex_trylock

当互斥锁锁住的时候，不阻塞，直接返回EBUSY错误。没锁住的时候则上锁，返回成功。

`int pthread_mutex_trylock(pthread_mutex_t *mutex);`

#### 返回值

成功：0

失败：正的错误值

### 互斥量属性

互斥量的属性使用pthread_mutexattr_t结构进行设置。

`int pthread_mutexattr_init(pthread_mutexattr_t *attr);`使用默认初始化

`int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);`反初始化

获取或设置attr中的有关share的值，可以为PTHREAD_PROCESS_PRIVATE（默认，线程间），PTHREAD_PROCESS_SHARED（表示进程间共享）

`int pthread_mutexattr_getpshared (const pthread_mutexattr_t *restrict attr,int *restrict pshared);`

`int pthread_mutexattr_setpshared (pthread_mutexattr_t *attr,int pshared);`

设置健壮性：当互斥量定义为shared时，其在进程间使用，当一个持有锁的进程终止时，由于他的锁没有释放，会影响其他进程/线程工作，如果设置属性为健壮，这种情况下会有通知，即在pthread_mutex_lock函数调用时，返回EOWNERDEAD。对于返回EOWNERDEAD的线程，如果直接解锁互斥量会导致该互斥量不再可用，如果想继续使用该互斥量，需要在解锁之前调用pthread_mutex_consistent函数，然后再解锁这样，互斥量就可以继续使用了。

`int pthread_mutexattr_getrobust(const pthread_mutexattr_t *restrict attr,int *restrict robust);`

`int pthread_mutexattr_setrobust(const pthread_mutexattr_t *restrict attr,int robust);`

`int pthread_mutex_consistent(pthread_mutex_t *mutex);`

定义类型：决定在进行，对没有解锁的互斥量加锁，对其他线程上锁的进行解锁，对已经解锁的进行解锁，三种操作时，会产生什么结果。四种类型。

`int pthread_mutexattr_gettype(const pthread_mutexattr_t *restrict attr,int *restrict type);`

`int pthread_mutexattr_settype(const pthread_mutexattr_t *restrict attr,int type);`

### 条件变量

条件变量也定义为一个全局变量，需要和互斥锁搭配使用。对于静态分配的不用调用destroy析构，对于动态分配的需要在free之前调用 destroy。但是对于静态分配的可以使用init进行初始化。

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;	//这里是静态初始化，还可以使用malloc分配，然后调用函数初始化

pthread_cond_t c = PTHREAD_COND_INITIALIZER;	//这里是静态初始化，还可以使用malloc分配，然后调用函数初始化

### pthread_cond_init/destroy

说明参见mutex和读写锁。

属性有进程共享属性和时钟属性，时钟属性用来指定timedwait函数的计时时钟。

### pthread_cond_wait

等待条件的发生。在获取互斥锁之后调用，该函数会先解锁然后阻塞等待条件发生（原子操作），解锁的目的是让其他线程访问互斥锁保护的变量。当条件发生时，退出阻塞并重新上锁。

如果在调用该函数前发生条件，则条件丢失。

`int pthread_cond_wait(pthread_cond_t *cptr,pthread_mutex_t *mptr);`

返回值如前。

### pthread_cond_timedwait

等待条件发生，如果超过abstime指定的绝对时间（自1970.01.01到现在）则退出。

`int pthread_cond_timedwait(pthread_cond_t *cptr,pthread_mutex_t *mptr,const struct timespec *abstime);`

返回值如前

### pthread_cond_signal

解阻塞等待条件的线程中的一个，

`int pthread_cond_signal(pthread_cond_t *cptr);`

返回值如前。

### pthread_cond_broadcast

解阻塞所有等待该条件的线程。

`int pthread_cond_broadcast(pthread_cond_t *cptr);`

### 读写锁

也叫共享互斥锁，共享指的是读模式，互斥指的是写模式。

存在读加锁，写加锁，不加锁三种状态。读加锁状态下，不阻塞其他读加锁，但会阻塞写加锁请求，并同时不允许新的读请求，当现有读加锁处理完后，进入写加锁状态。写加锁状态下，会阻塞所有读/写加锁请求。

对于静态分配的不用调用destroy析构，对于动态分配的需要在free之前调用 destroy。但是对于静态分配的可以使用init进行初始化。

### pthread_rwlock_init

用来初始化读写锁，读写锁使用之前必须进行初始化。对于静态分配的读写锁，如果默认属性足够的话，可以使用PTHREAD_RWLOCK_INITTIALIZER进行初始化。

读写锁的属性只要进程共享属性，设置类似mutex。

`int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock,const pthread_rwlockattr_t *restrict attr);`

**rwlock**:需要初始化的读写锁的指针，可以是静态声明的变量取地址得到的，也可以是使用malloc动态分配的。需要调用pthread_mutex_destroy。

**attr**:读写锁的属性，如果设置为NULL，则使用默认的进行初始化。

#### 返回值

成功：0

失败：正的错误值

### pthread_rwlock_destroy

用来destroy读写锁（析构）

`int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);`

**rwlock**:需要destroy的读写锁的指针

#### 返回值

成功：0

失败：正的错误值

### pthread_rwlockattr_init/destroy

用来使用默认属性初始化属性变量/析构一个属性变量

`int pthread_rwlockattr_init(pthread_rwlockattr_t *attr);`

`int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);`

#### 返回值
同上

### pthread_rwlockattr_get/setpshared

设置属性的值，当前只有PTHREAD_PROCESS_PRIVATE/SHARED

`int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *attr,int *valptr);`

`int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr,int value);`

#### 返回值
同上

### pthread_rwlock_rdlock/wrlock/unlock

用来加读锁，加写锁，解锁（包括读锁和写锁）

`int pthread_rwlock_rdlock/wrlock/unlock(pthread_rwlock_t *rwlock);`

**rwlock**:读写锁变量的指针。

#### 返回值

成功：0

失败：正的错误值

#### 说明

`int pthread_rwlock_tryrdlock/wrlock(pthread_rwlock_t *rwlock);`

功能类似于pthread_mutex_trylock函数，尝试获取锁，如果不能马上获得，返回一个EBUSY错误。

`int pthread_rwlock_timedrdlock/wrlock(pthread_rwlock_t *restrict rwlock,const struct timespec *restrict tsptr);`

带有超时的读写锁，指定的是绝对的时间。

上述两个函数的返回值，和上面其他函数的返回值一样。

### 自旋锁

和互斥锁类似，使用方法和函数接口也基本相同，只要将mutex更换为spin即可。

二者的区别在于，在等待锁时mutex会进入睡眠状态，不占用CPU；而spin会进入一个循环等待，还会占用CPU，所以只适合等待锁时间不长的应用中。

### 屏障

等待指定数量的线程到达某一点，然后会对其中的一个线程返回PTHREAD_BARRIER_SERIAL_THREAD（其他返回0），该线程可用于集中结果处理。也可以不用返回PTHREAD_BARRIER_SERIAL_THREAD的线程做结果处理，可以使用主线程做处理。

属性：只有进程共享属性。

### pthread_barrier_init

`int pthread_barrier_init(pthread_barrier_t *restrict barrier,const pthread_barrierattr_t *restrict attr,unsigned int cout);`

**count**:指定允许所有线程继续运行之前，必须到达屏障的线程数目。

### pthread_barrier_destroy

###  pthread_barrier_wait

用来增加屏障技术，如果增加后没有达到要求，那么进入休眠状态。

`int pthread_barrier_wait(pthread_barrier_t *barrier);`

#### 返回值

成功：0或PTHREAD_BARRIER_SERIAL_THREAD

失败：正的错误值

#### 说明

到达屏障计数值后，并且线程处于非阻塞状态，则屏障可以被重用。除非destroy并进行新的初始化，否则屏障计数不会改变。

## 处理结构的整体函数
这些函数将套接字当做以字节为单位的数组，进行操作。有两组具有相同功能的函数。

### 改值函数`bzero,memset`
头文件为`strings.h`  
函数声明：  
`void bzero(void *dest,size_t len);`
`void *memset(void *dest,int c,size_t len);`  
功能：用于将dest所指向数组的len个元素变成数值c(bzero,c=0);  
`dest`:目标数组的头指针  
`len`:想要转换的元素个数  

### 复制函数`bcopy,memcpy`
函数原型：  
`void bcopy(const void *src,void *dest,size_t nbytes);`  
`void *memcpy(void *dest,const void *src,size_t nbytes);`  
功能：将nbytes的src中的元素复制到dest  
注意：memcpy不能处理dest和src相等的情况。  

### 比较函数`bmp,memcmp`
函数原型：  
`int bcmp(cosnt void *front,const void *last,size_t nbytes);`  
`int memcmp(cosnt void *front,const void *last,size_t nbytes);`  
功能：如果front>last返回正数，相等返回0，小于返回负数.
  
## IP地址转化的函数
在ASCII字符和网络字节序的二进制值（存放在套接字地质结构中的值）之间进行转换。

### IPv4转化函数
头文件`arpa/inet.h`  
这些函数只能用于IPv4的转换，目前更多使用的是后面要说的IP地址转换函数   
`int inet_aton(const char *strptr,struct in_addr *addrptr);`   
功能:该函数用来将点分十进制的IP地址转换为32位的网络字节的IPv4地址。  
`strptr`存储点分十进制的值；`addrptr`存储转换后的结果  
成功返回1，失败返回0   
注意：当`addrptr`为空指针时，函数不向其中写入任何值。  
  
`in_addr_t inet_addr(const char *strptr);`  
该函数直接将结果返回。出错时返回INADDR_NONE(全1)，容易引发问题，已经被弃用。  
  
`char *inet_ntoa(struct in_addr inaddr);`  
注意：1.该函数返回值存在静态内存中，所以该函数不能重入。

### IPv4&IIPv6通用的转换函数
头文件`arpa/inet.h`  
`int inet_pton(int family,const char *strptr,void *addrptr);`  
功能：将字符串的IP地址转化为存储在addrptr中的数值地址  
family:可以是AF_INET或AF_INET6  
成功返回：1
失败返回：0，若输入的字符串不是有效的表达式只返回0；  
失败返回：-1，当family为不支持的地址族同时将errno置为EAFNOSUPPORT.  
  
`const char *inet_ntop(int family,const void *addrptr,char *strptr,size_t len);`  
功能：将存储在addrptr中的函数转换为存储在strptr中的字符串。  

**addrptr**:这个对应的不是struct sockaddr *,而是对struct sin_addr *

成功返回：strptr的值  
失败返回：空指针，当len太小，同时置errno为ENOSPC；当family为不支持的地址族同时将errno置为EAFNOSUPPORT  
注意：为了保证len足够在`netinet/in.h`中定义了如下的两个值，用于指定len  
```c
#define INET_ADDRSTRLEN 16
#define INET6_ADDRSTRLEN 46
```  
以上的定义表示字节数  

## 调整字节顺序的函数
对于主机字节序和网络字节序不同的，将两种字节序（大端小端）之间相互转换。

### 主机转到网络
头文件`netinet/in.h`  
`uint16_t htons(uint16_t host16value);`  
`uint32_t htonl(uint32_t host32value);`  
分别用于将16位和32位的主机字序的数据转换为网络字节序。  
  
### 网络到主机
头文件`netinet/in.h`  
`uint16_t ntohs(uint16_t net16value);`  
`uint32_t ntohl(uint32_t net32value);`  
分别用于将16位和32位的网络字节序的数据转换为主机字节序。  

注意：即使对于l为64个字节的主机来说，也是转换32位的。
