# Socket

本节介绍构建网络应用程序所需要的基本函数的头文件，原型，用法，并通过一个简单的客户端程序和服务器程序，说明函数的使用。

程序用到了大多文档中涉及的函数，实现了从客户端和主机获取连接的IP地址，并通过套接字实现了IP地址的传递。

## 好习惯
1.在为套接字地址的family，address，port赋值之前，要调用bzero为齐置零

## 错误整理

1.if(sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP) == -1)这样的调用是不正确的应该像下面这样调用：

if((sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == -1)

因为这里存在优先级问题。否则可能产生**errno == 88**,即**socket operation on non-socket**错误。可能不在这行函数中报错。如可能在connect函数处报错

2.错误：忘记给servAddr.sin_family参数赋值

3.bind函数出错。原因有错误1中调用socket的错误，以及端口号选择15存在些问题。

4.write写函数是按照一个字节往下写的,如果传入int型仅int的最低位被赋值,最终验证结果不正确.

5.对于main函数传入参数理解不正确。argv[1]才是调用时传给函数的第一个参数。argv[0]是程序所在的完整路径

6.调用客户函数一定要输入服务器的地址，如在本机运行输入127.0.0.1

7.
```
$(objectname) : %:%.c
    $(CC) $< $(CFLAGS) -o $@
```

只有按照上面这样的写法，$<才每次一个的顺序取出objectname中.c文件，生成对应的可执行文件。

```
$(objectname) : $(objects)
    $(CC) $< $(CFLAGS) -o $@
```

仅会取出objects的第一元素名，用它逐个生成objectname中的所有可执行文件。

