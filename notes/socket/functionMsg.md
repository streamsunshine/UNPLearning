## 套接字函数
介绍套接字编程的基本函数。  
### socket函数

用来生成一个对于指定协议的套接字描述符，通过套接字描述符才可以实现对套接字的操作。

#### 头文件：

`sys/socket.h`

#### 函数

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

