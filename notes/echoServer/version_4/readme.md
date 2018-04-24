## 版本4

在版本3的基础上，为connect函数，接收函数添加超时功能。

对于connect函数使用alarm技术，对于select要求对应套接字工作在非阻塞模式。

对于发送，接收函数，可以使用alarm，select，以及套接字选项SO_RCVTIMEO和SO_SNDTIMEO。

### alarm

利用linux的信号处理过程,在调用可能超时的函数前,为SIGALRM设置信号处理函数.并启动计时,当函数超时会被中断,此时errno等于EINT,通过检测可知函数超时.

### select

利用select函数自身的超时控制机制.如果超时select函数返回0

### 套接字选项

利用套接字选项性质,可以一次性设置,以后的接收和发送函数都具有超时控制机制。设置后，对应的读/写函数如果超时，会时errno的值为EWOULDBLOCK.
