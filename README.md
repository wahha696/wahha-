# wahha聊天室

## 简介

+ 使用TCP通信协议
+ 定义消息结构体MSG，包含消息类型type(LOGIN CHAT QUIT)，消息文本，用户名称。发送时将MSG序列化，转化成一个字符串，接收时再反序列化解析出来
+ 封装了两个类：ChatServer ChatClient

## ChatServer

### 成员变量：

+ int server_sockfd：服务器套接字

+ std::map<int, Client> clients：在线的客户端列表，使用红黑树，增加查找效率
+ std::mutex clients_mutex：保护客户端列表的互斥锁
+ whThreadPool* threadPool：线程池指针

### 构造函数：

+ 功能：创建服务器套接字、绑定服务器结构体信息、启动监听、启动线程池
+ 参数：
	+ ip：服务器IP地址
	+ port：服务器开启的端口号
	+ threadPoolSize：线程池的最小线程数

### handleClient

+ 功能：处理客户端的任务函数，放入线程池中作为新的任务
+ 参数：
	+ client_sockfd：与客户端通信的套接字
	+ client_addr：客户端的结构体地址信息

### broadcast

+ 功能：辅助函数，向所有在线的客户端发送消息，实现聊天室功能
+ 参数：
	+ msg：要发送的消息
	+ exclude_sockfd：向除了该套接字的客户端发送信息，默认为-1，即向全体发送

### run

+ 功能：启动整个服务器

### 关于线程池：

[whThreadPool](https://github.com/wahha696/whThreadPool)

## ChatClient

### 成员变量

+ int client_sockfd：客户端套接字
+ std::string m_name：客户端名字
+ bool running：客户端是否在运行

### 构造函数

+ 功能：创建套接字，连接服务器
+ 参数：
	+ ip：服务器IP地址
	+ port：服务器开启的端口号
	+ name：客户端名称

### sendMsg

+ 功能：向服务器发送信息
+ 参数：
	+ type：信息种类
	+ content：信息正文

+ 返回值：bool类型，是否成功

### recvMsg

+ 功能：作为接收服务器信息的线程体

### run

+ 功能：运行客户端，读取标准输入发送到服务器，开启线程接收服务器信息


