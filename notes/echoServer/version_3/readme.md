## 版本3

通过主机名，服务名，使用DNS获取IP地址，端口号。

### 说明1

gethostbyname,gethostbyaddr,getservbyname,getservbyport,这些函数只能用于IPv4的情况，同时是任意两个之间是不可重入的，所以我们采用了getaddrinfo和getnameinfo两个函数，它们只要在其内部调用的函数是可重入的时，就是可重入的，同时它们使用IPv4和IPv6。

### 说明2

代码调试时如何使用netstat。

netstat -pa | grep echoServ  //能够得到当前运行的服务器程序的端口号，PID的情况。其中p表示显示程序名称，a表示显示所有。

本程序直接受用了echo的众所周知端口号（7），通过netstat可以观察到。

### 相关的linux知识

/etc/networks 网络的符号名称

/etc/nsswitch.conf 用来规定查找信息的途径和顺序。files表示在本地文件查找，dns 访问dns服务器，nis搜索nis数据库.compat控制passwd,group,shadow中指令的

/etc/resolv.conf 配置DNS服务器

/etc/hosts 存储本地主机名称到地址的映射

/etc/services 存储服务到服务端口号的映射
