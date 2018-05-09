## version_1

客户端：使用阻塞式I/O运行停等协议。端口9600

服务器：使用多进程技术，并对退出的子线程产生的SIGCHLD信号调用waitpid进行处理。

## version_2

客户端：使用select函数解决一些服务器异常的问题。

服务器：使用select函数实现。

## version_3

客户端：使用getaddrinfo函数获取IP地址等信息，进行套接字创建等操作。

服务器：使用getaddrinfo获取地址等。

函数实现基于version_1，是进程实现方式。

## version_4

在version_3的基础上，为connect等阻塞式I/O实现超时退出机制。

## version_5

以version_2为基础，实现了unix域套接字的实验。
