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

成功：返回信号处理函数指针

失败：返回SIG_ERR（-1）

<<<<<<< HEAD
=======
### select

头文件:sys/select.h sys/time.h

`int select(int maxfdp1,fd_set *readset,fd_set *writeset,fd_set *exceptset,const struct timeval *timeout);`

当readset中描述符可读，writeset中的可写，exceptset中的有错误是select函数返回

**maxfdp1**:三个描述符集合中，描述符最大值加1的值。告知内核检查到哪一个描述符。

**readset**：需要检查可读的描述符集合

**timeout**：如果select阻塞时间超过了timeout规定值，select函数返回

#### 返回值

情况1：如果有就绪的，返回就绪描述符个数

情况2：超时，返回0

情况3：出错，返回-1

#### 用法

参见 ../echoServer/version_2/echoCli.c

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
>>>>>>> develop

