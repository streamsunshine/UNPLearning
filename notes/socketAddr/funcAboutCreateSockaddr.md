## 处理结构的整体函数
这些函数将套接字当做以字节为单位的数组，进行操作。有两组具有相同功能的函数。

### 改值函数`bzero,memset`
头文件为`strings.h`  
函数声明：  
`void bzero(void *dest,size_t len);`
`void *memset(void *dest,int c,size_t len);`  
功能：用于将dest所指向数组的len个元素变成数值c(bzero,c=0);  
`dest`:目标数组的头指针  
`len`:想要转换的元素个数  

### 复制函数`bcopy,memcpy`
函数原型：  
`void bcopy(const void *src,void *dest,size_t nbytes);`  
`void *memcpy(void *dest,const void *src,size_t nbytes);`  
功能：将nbytes的src中的元素复制到dest  
注意：memcpy不能处理dest和src相等的情况。  

### 比较函数`bmp,memcmp`
函数原型：  
`int bcmp(cosnt void *front,const void *last,size_t nbytes);`  
`int memcmp(cosnt void *front,const void *last,size_t nbytes);`  
功能：如果front>last返回正数，相等返回0，小于返回负数.
  
## IP地址转化的函数
在ASCII字符和网络字节序的二进制值（存放在套接字地质结构中的值）之间进行转换。

### IPv4转化函数
头文件`arpa/inet.h`  
这些函数只能用于IPv4的转换，目前更多使用的是后面要说的IP地址转换函数   
`int inet_aton(const char *strptr,struct in_addr *addrptr);`   
功能:该函数用来将点分十进制的IP地址转换为32位的网络字节的IPv4地址。  
`strptr`存储点分十进制的值；`addrptr`存储转换后的结果  
成功返回1，失败返回0   
注意：当`addrptr`为空指针时，函数不向其中写入任何值。  
  
`in_addr_t inet_addr(const char *strptr);`  
该函数直接将结果返回。出错时返回INADDR_NONE(全1)，容易引发问题，已经被弃用。  
  
`char *inet_ntoa(struct in_addr inaddr);`  
注意：1.该函数返回值存在静态内存中，所以该函数不能重入。

### IPv4&IIPv6通用的转换函数
头文件`arpa/inet.h`  
`int inet_pton(int family,const char *strptr,void *addrptr);`  
功能：将字符串的IP地址转化为存储在addrptr中的数值地址  
family:可以是AF_INET或AF_INET6  
成功返回：1
失败返回：0，若输入的字符串不是有效的表达式只返回0；  
失败返回：-1，当family为不支持的地址族同时将errno置为EAFNOSUPPORT.  
  
`const char *inet_ntop(int family,const void *addrptr,char *strptr,size_t len);`  
功能：将存储在addrptr中的函数转换为存储在strptr中的字符串。  
成功返回：strptr的值  
失败返回：空指针，当len太小，同时置errno为ENOSPC；当family为不支持的地址族同时将errno置为EAFNOSUPPORT  
注意：为了保证len足够在`netinet/in.h`中定义了如下的两个值，用于指定len  
```c
#define INET_ADDRSTRLEN 16
#define INET6_ADDRSTRLEN 46
```  
以上的定义表示字节数  

## 调整字节顺序的函数
对于主机字节序和网络字节序不同的，将两种字节序（大端小端）之间相互转换。

### 主机转到网络
头文件`netinet/in.h`  
`uint16_t htons(uint16_t host16value);`  
`uint32_t htonl(uint32_t host32value);`  
分别用于将16位和32位的主机字序的数据转换为网络字节序。  
  
### 网络到主机
头文件`netinet/in.h`  
`uint16_t ntohs(uint16_t net16value);`  
`uint32_t ntohl(uint32_t net32value);`  
分别用于将16位和32位的网络字节序的数据转换为主机字节序。  

注意：即使对于l为64个字节的主机来说，也是转换32位的。
