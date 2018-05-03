## 版本6

程序在版本2的基础上做出修改。版本2相对于停等方式更具有并行性，但是由于使用阻塞式函数，会让读写函数阻塞，来等待资源可用，这就浪费了时间。因此考虑使用非阻塞I/O，但是代码过于复杂，常用的办法是使用多进程或者多线程。

当我们发现需要使用非阻塞I/O时，更简单的办法通常是把应用程序任务划分到多个进程或线程。

执行速率方面：非阻塞I/O  线程版本 进程版本 select加阻塞式I/O 停等版本

### 客户程序的修改

#### 修改1

使用进程化的版本处理发送和接收。使用一个进程从控制台读数据并发数据；使用另一个进程从套接字收数据并写到控制台。

#### 修改2

使用非阻塞的connect实现一个应用程序和服务器构建多个连接，从而一次可以传输多个数据。

这样做在网络存在拥塞时可能造成负面影响。因为一个连接拥塞后，其余的连接不能够得到通知，它们继续发送的分组会进一步影响网络性能。

### 服务器程序的修改

#### 修改1

将accept修改成非阻塞的。在select检测到某个监听套接字已准备好被accept时，把套接字设置为非阻塞的。并忽略其EWOULDBLOCK，ECONNABORTED，EPROTO以及EINTR错误。

原因：如果在select和accept之间，客户端终止了连接请求，对应accept函数就会阻塞，直到下次该监听套接字上有请求到来。这就导致了服务器不能做其他工作，所以需要做修改。