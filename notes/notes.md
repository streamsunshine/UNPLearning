## 笔记

### IPv4和IPv6的互操作

其地址变换操作应该在TCP和IP层之间的接口函数，如connect，accept进行处理。

#### IPv4客户端访问IPv6服务器

IPv4客户端得到A类型的DNS的资源条目，即得到服务器的IPv4地址。调用connect函数，当connect请求到达服务器时，服务器设置一个标志，指示本连接使用IPv4映射的IPv6地址，并由accept返回映射的IPv6地址。

当服务器向客户端发送信息时，服务器将映射的IPv6地址转换为IPv4地址。

注：

1.数据传输使用的IPv4

2.对UDP也是同样的道理

3.服务器应该是双栈的IPv6服务器

4.这种分析的假设是服务器同时用IPv4地址和IPv6地址。

#### IPv6客户端访问IPv4服务器

此时DNS中应该有服务器从IPv4到IPv6的映射条目。同时IPv4的地址获取函数getaddrinfo应该设置AI_V4MAPPED。内核检测到使用IPv4映射的IPv6地址时，会发送一个IPv4的分节，从而实现通信。

注：

1.数据传输使用的仍然是IPv4

2.对UDP同样适用

3.要求客户机是双栈的，对于服务器不要求。

#### 补充

上述两种通信方式，客户端和服务器都不知道与其通信的对端采用哪一种协议，这种问题可以使用netinet.h中的宏进行判定。

例如：IPv6客户可以调用IN6_IS_ADDR_V4MAPPED(const struct in6_addr *aptr);判定解析器返回的IPv6地址是不是映射得到的。
