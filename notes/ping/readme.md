## ping

这里按照unix网络编程中的叙述，实现一个可以使用的ping程序，由于IPv4和IPv6的函数基本为分开实现的，所以这里只实现IPv4版本的程序。

### 要点

1、在发送icmp时，IP头由内核构造，我们只负责设置icmp的结构，所以发送缓冲区存储的是icmp的结构。而在接收icmp时，内核向进程传递的是整个IP数据报，所以接收缓冲区中存储的是整个IP的结构，要获得icmp的结构要加上ip头的偏移。

2、这里处理的ICMP回射服务，将进程号和当前时间作为ICMP的可选数据发送给服务器，以便于识别发送进程，并计算RTT。

3、这里尽量将程序编写成独立的函数，对错误进行一定的封装。

4、这里涉及到对IP数据报和ICMP数据报的操作，相当于在底层进行操作，区别于TCP直接操作数据，这里要关注IP协议和ICMP协议的格式。

### bug

1、sendmsg返回errno = 105，原因是在msghdr在使用前，没有调用bzero置零。

2、发送出去帧后，接收不到返回帧。但是如果在同一台主机上调用主机的ping函数，则我们编写的程序能够收到帧。通过双机测试，发现我们的帧实际上发出去了。

最后检测结果为校验和计算错误。result = (result & 0xffff) + result >> 16;可见由于+的优先级高于>>所以，最终得到的结果并不正确，所以需要给result >> 16 添加括号。


