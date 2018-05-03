头文件：sys/un.h
```c
/* Structure describing the address of an AF_LOCAL (aka AF_UNIX) socket.  */
struct sockaddr_un
  {
    __SOCKADDR_COMMON (sun_);
    char sun_path[108];		/* Path name.  */
  };
```
结构体的第一行表示，地址类型。如果具体化的第一行应为：

sa_family_t sun_family； 

unix域协议是IPC的一种方式，所以套接字地址类型是AF_LOCAL

而sun_path在各个实现中的大小是不确定的,使用时应该使用sizeof(a.sun_path)进行检验。

bind中的未指定地址（任意地址）使用sun_path[0] = 0;表示。

bind函数传入地址大小时，应该传入sockaddr_un内有效数据的大小，通过使用SUN_LEN(&a)获取。

用来向sun_path中拷贝数据的函数strncpy(a.sun_path,sStr,sizeof(a.sun_path)-1);//使用前先对sun_path初始化为0；
