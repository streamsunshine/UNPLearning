## 套接字函数
介绍套接字编程的基本函数。如未特殊说明，头文件均为**sys/socket.h**。   
### socket函数

用来生成一个对于指定协议的套接字描述符，通过套接字描述符才可以实现对套接字的操作。

`int socket(int family,int type,int protocol);`

#### family:协议族（协议域）

|family|说明|
|----|------|
|AF_INET|IPv4协议|
|AF_INET6|IPv6协议|
|AF_LOCAL|Unix域协议|
|AF_ROUTE|路由套接字|
|AF_KEY|密钥套接字|

#### type：套接字类型

|type|说明|
|----|-----|
|SOCK_STREAM|字节套接字|
|SOCK_DGRAM|数据套接字|
|SOCK_SEQPACKET|有序分组套接字|
|SOCK_RAW|原始套接字|

#### protocol：协议类型

|protocol|说明|
|----|-----|
|IPPROTO_TCP|TCP传输协议|
|IPPROTO_UDP|UDP传输协议|
|IPPROTO_SCTP|SCTP传输协议|

#### 注意

这三个参数之间是存在相关关系的。对于TCP协议只能有：IPv4/IPv6 SOCK_STREAM TCP两种组合方式。

#### 返回值

成功：返回int型的套接字描述符

失败：返回-1

### connect函数

用于发起连接，如会激发TCP的三路握手

`int connect(int sockfd,const struct sockaddr *servaddr,socklen_t addrlen);`

**sockfd**:为socket函数返回的套接字描述符； **servaddr**:为服务器的地址（必须包含IP和Port）  **addrlen**:地址结构大小

#### 返回

成功：返回0

失败：返回-1。没收到syn的相应，errno=ETIMEDOUT； 收到RST，说明主机端口没有进程等待，errno=ECONNREFUSED;  路由产生“destination unreachable”，errrno=EHOSTUNREACH或者ENETUNREACH

#### 注意

如果connect失败，该套接字必须关闭，不能再次对其调用connect函数

### bind函数

用于将协议地址赋予一个套接字，可以赋予一个通用地址标志符，则套接字实际地址由内核分配。

`int bind(int sockfd,const struct sockaddr *myaddr,socklen_t addrlen);`

**sockfd**:为socket函数返回的套接字描述符； **myvaddr**:为协议地址指针（必须包含IP和Port）  **addrlen**:地址结构大小

#### 返回值

成功： 返回0

失败： 返回-1.常见错误errno=EADDRINUSE

#### 注意

1.对于IPv4地址通配符使用INADDR_ANY，其值为零，端口通配符也是0；对于IPv6其地址通配符是一个结构体，为in6addr_any，在netinet/in.h中定义

2.通配端口在bind时就已经分配，而IP要在套接字已连接或发出数据报时，才选择**本地IP地址**

3.本地IP地址指的是本地主机有多个网口接入时，其多个IP中的一个。

### listen函数

用于将socket创建的主动套接字（发起连接的）转换为被动套接字（等待连接的）

`int listen(int sockfd,int backlog);`

**sockfd**:为socket函数返回的套接字描述符； **backlog**:不同实现的定义不同；现一般定义为与已连接队列大小相关的量；为保证服务一般这个值较大

#### 返回值

成功： 返回0

失败： 返回-1

#### 注释

在套接字建立连接时，服务器会为已经接收到syn的套接字建立一个未完成连接队列，对于完成三路握手后的会建立一个已完成连接队列。

### accept函数

根据监听套接字的设置，从已完成连接队列头部取出一个已完成连接，如果没有则进程进入睡眠（设置阻塞）

`int accept(int sockfd,struct sockaddr *cliaddr,socklen_t *addrlen);`

**sockfd**:监听套接字描述符；   **cliaddr**:客户套接字地址；    **addrlen**：套接字地址大小

#### 返回值

成功： 返回一个非负的已连接套接字描述符
失败： 返回-1

#### 注意

1.一个服务器一般只有一个监听套接字。对于已连接套接字，服务器会为每个客户端请求的连接生成一个，服务完成后将其关闭（需要手动调用close）

2.如果对于客户的地址不感兴趣，可以将cliaddr和addrlen设置为空指针。

### close函数

将套接字标记为关闭，系统会在发送完所有已排队数据后，执行终止序列

`int close(int sockfd);`

**sockfd**:想要终止的套接字描述符

#### 注释

进程在结束后会关闭所有描述符，利用这一点可以不调用close函数。但对于并发服务器，父进程必须调用close关闭传递给子进程的套接字，否则可能会出现套接字描述符不足；

close实际上另套接字的计数值减一，当计数值为零时才执行，close函数所指定的操作。

### getsockname函数

用于获取内核为套接字自动分配的**本地IP和端口号**

`int getsockname(int sockfd,struct sockaddr *localaddr,socklen_t *addrlen);`

**sockfd**:获取地址的套接字描述符； **sockaddr，addrlen**：存储返回的地址，地址大小的值-结果参数

#### 返回值

成功： 返回0

失败： 返回-1

### getpeername函数

用于获取已连接套接字描述符对应的对端的地址和端口号。当函数间只传递已连接套接字描述符时，可以通过该函数获取对端的地址和端口号

`int getpeername(int sockfd,struct sockaddr *peeraddr,socklen_t *addrlen);`

**sockfd**:获取地址的套接字描述符； **sockaddr，addrlen**：存储返回的地址，地址大小的值-结果参数

