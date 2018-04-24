## 笔记

### IPv4和IPv6的互操作

其地址变换操作应该在TCP和IP层之间的接口函数，如connect，accept进行处理。

#### IPv4客户端访问IPv6服务器

IPv4客户端得到A类型的DNS的资源条目，即得到服务器的IPv4地址。调用connect函数，当connect请求到达服务器时，服务器设置一个标志，指示本连接使用IPv4映射的IPv6地址，并由accept返回映射的IPv6地址。

当服务器向客户端发送信息时，服务器将映射的IPv6地址转换为IPv4地址。

注：

1.数据传输使用的IPv4

2.对UDP也是同样的道理

3.服务器应该是双栈的IPv6服务器

4.这种分析的假设是服务器同时用IPv4地址和IPv6地址。

#### IPv6客户端访问IPv4服务器

此时DNS中应该有服务器从IPv4到IPv6的映射条目。同时IPv4的地址获取函数getaddrinfo应该设置AI_V4MAPPED。内核检测到使用IPv4映射的IPv6地址时，会发送一个IPv4的分节，从而实现通信。

注：

1.数据传输使用的仍然是IPv4

2.对UDP同样适用

3.要求客户机是双栈的，对于服务器不要求。

#### 补充

上述两种通信方式，客户端和服务器都不知道与其通信的对端采用哪一种协议，这种问题可以使用netinet.h中的宏进行判定。

例如：IPv6客户可以调用IN6_IS_ADDR_V4MAPPED(const struct in6_addr *aptr);判定解析器返回的IPv6地址是不是映射得到的。


### 守护进程

守护进程是后台运行且不与任何控制终端关联的进程。1、由系统初始化脚本启动。2、由控制终端启动，但要脱离与控制终端的关联。这里介绍两种守护进程，rsyslogd和xinetd，分别用于日志管理和服务器管理。

许多网络服务器作为守护进程运行。并将必要输出的信息传递给syslogd进行记录。

#### rsyslogd

rsyslogd是syslogd的升级版。配置文件为/etc/rsyslog.conf，可以直接指定各种日志消息应该如何处理,也可以通过另外包含配置文件的方式指定。语法可以参见鸟哥私房菜的第十九章.

可以通过unix域数据报套接字,udp套接字,以及/dev/klog端口的方式,从自己的守护进程向syslogd发送日志消息.但通常使用syslog函数.无论采取哪一种方式，效果都是一样的，日志消息最终存储在哪一个文件中取决于/etc/rsyslog文件中的配置。


`void syslog(int priority,const char *message,argument);`

头文件：syslog.h

priority:是level和facility二者的组合，未指定level时，level默认为LOG_NOTICE.

message:类似于printf的格式化字符串，但是增加了%m功能。%m将被替换成当前errno值对应的出错信息。

argument:上面格式化字符串的变量。

例子：

syslog(LOG_INFO | LOG_LOCAL2,"This is a message of %s\n",argv[0]);表示发送信息的进程类型为本地使用，级别为通告信息。最终输出结果为This is a message of 程序名。


`void openlog(const char *ident,int options,int facility);`

该函数在syslog前运行，帮助syslog打开unix域数据报套接字。如果不调用，sysylog将在第一次被调用时，自己打开。

ident:放在每个日志消息之前的字符串，一般为程序名

options：指定打开unix域数据报套接字的一些性质（参见头文件）

facility:如果在syslog中没有指定的facility，其作为默认值。

`void closelog(void);`  关闭unix域数据报套接字。

### 将服务器进程转换为守护进程

需要脱离shell，同时做一些其他的处理，但是我们对这个操作并不急需，同时有一些并不理解的原理，所以等到需要的时候再做补充。

### xinetd守护进程

使用超级服务器可以通过单个进程为多个服务等待外来的客户请求，可以减少系统的进程数。
xinetd根据自己配置文件/etc/xinetd.conf决定处理哪些服务，以及当服务请求到达时怎么处理.

通过xinetd管理服务器时，xinetd函数会运行初始的绑定，listen，以及select监测listen端口，当有请求到来时，会调用accept并创建一个子进程，在子进程中将stdin，stdout，stderr都绑定到accept返回的套接字上面。然后切换用户，并调用exec。因此我们编写的程序中，stdin，stdout，stderr就相当于套接字，我们对它们进行输入输出即可。

nowait:表示应用进程不必等待某个子进程终止，就可以接受另一个连接，如tcp；

wait：表示应用进程必须等待子进程终止才能为下一个请求服务，即在子进程结束之前要关闭xinetd的select中，对于该套接字的监测。如udp需要设为wait。

注：xinetd不适用于服务密集型服务器，因为对于每个服务既要有fork的开销，又要有exec的开销。

### 标准I/O函数库

由于标准I/O库对于除了标准错误外的其他I/O流默认使用全缓冲，所以一般要避免在套接字上面使用标准I/O库函数。

完全缓冲：在缓冲区满、调用fflush函数或者调用进程调用exit函数终止时才输出。

行缓冲；不缓冲；

### T/TCP 事务目的TCP

是一种通过结合SYN，FIN，数据分节以减少RTT的TCP协议的修改版本。

客户通过调用sendto结合套接字选项和函数选项，而避免调用connect，shutdown函数，从而节省时间。

事务：含义是客户的请求和服务器的应答。如DNS，，HTTP
