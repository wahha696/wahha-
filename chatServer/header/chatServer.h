#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H
#include <iostream>
#include <cstring>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>
#include <map>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../../ThreadPool/whThreadPool.h"


#define MSG_BUFFER_SIZE 128 // 消息缓冲区的大小
#define LOGIN 1             // 登录消息类型
#define CHAT 2              // 聊天消息类型
#define QUIT 3              // 退出消息类型

class ChatServer
{
public:
    /**
     * @brief 构造函数，初始化服务器套接字，绑定地址，监听端口，并启动线程池
     * @param ip 服务器IP地址
     * @param port 服务器端口号
     * @param threadPoolSize 线程池大小，默认为4
     */
    ChatServer(const char* ip, int port,int threadPoolSize = 4);

    /**
     * @brief 析构函数
     */
    ~ChatServer();

    /**
     * @brief 启动服务器
     */
    void run();

    /**
     * @brief 处理客户端的函数
     * @param client_sockfd 客户端套接字
     * @param client_addr 客户端地址
     */
    void handleClient(int client_sockfd,struct sockaddr_in client_addr);

    


    //消息结构体
    struct MSG
    {
        int type;            // 消息类型
        std::string name;    // 客户端名称
        std::string content; // 消息内容

        //序列换函数，将消息结构体转化为二进制，便于传输
        std::string serialize() const
        {
            return std::to_string(type) + "|" + name + "|" + content;
        }

        //反序列化函数，将二进制数据转化为消息结构体
        void deserialize(const std::string &data)
        {
            size_t pos1 = data.find('|');
            size_t pos2 = data.find('|', pos1 + 1);
            type = std::stoi(data.substr(0, pos1));
            name = data.substr(pos1 + 1, pos2 - pos1 - 1);
            content = data.substr(pos2 + 1);
        }
    };

    
    //客户端结构体
    struct Client
    {
        int sockfd;          // 客户端套接字
        struct sockaddr_in addr; // 客户端地址
    };

    void broadcast(const MSG& msg, int exclude_sockfd = -1); // 广播消息给所有在线的客户端，排除发送者
private:
    int server_sockfd; // 服务器套接字
    std::map<int, Client> clients; // 在线的客户端列表
    std::mutex clients_mutex; // 保护客户端列表的互斥锁

        //错误信息日志
    void errLog(const char* msg);

    whThreadPool* threadPool; // 线程池指针

};

#endif