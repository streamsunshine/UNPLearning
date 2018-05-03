## UDP套接字编程

UDP是一种无连接，不可靠的传输层协议，在快速传输超过缓冲区大小的数据时，容易出现丢包现象(即无流量控制)。但是它实现了最基本的网络通信功能，赋予用户更高的灵活性。

常见的应用程序有DNS，NFS等。

这里将实现一个UDP版本回射服务器程序。使用的函数参照../socket/functionSys.md中的说明

### 说明

#### 使用sendto函数的程序：

1.如未绑定IP和端口，会在第一次调用sendto时由操作系统赋予，并不在改变

2.发生异步错误不会立即通知，因为只有已连接（调用connect）的套接字才会立即通知。

3.sendto每次调用都要指定地址，一个套接字可以将信息发到不同的目的

4.调用sendto，两次调用每次都要经历建立连接，发送，断开连接的过程。而connect不用

#### 调用connect的版本

1.UDP的connect函数不同于TCP的，并不存在三次握手。

2.相当于构建了一个到指定服务器地址的连接，此时不能使用sendto，需要使用write。

3.能够实现错误立即通知。

4.对于来自非连接的数据直接丢弃。所以也不必调用recvfrom，直接使用read。

5.调用connect限制了客户端和服务器能够交换数据端的数量，一个套接字只能连接一个对端。

#### 多次调用connect

区别于TCP，UDP一个套接字可以多次调用connect，可以处于两个目的

1.指定一个新的IP和端口

2.断开已有连接。先将一个地址结构清零，然后赋予sin_family = AF_UNSPEC.