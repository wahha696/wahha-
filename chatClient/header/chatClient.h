#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H
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
#define MSG_BUFFER_SIZE 128 // 消息缓冲区的大小
#define LOGIN 1             // 登录消息类型
#define CHAT 2              // 聊天消息类型
#define QUIT 3              // 退出消息类型

class ChatClient
{
public:

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

    ChatClient(const char* ip,int port,const std::string& name);

    ~ChatClient();

    void run();

    /**
     * @brief 这是普通成员函数，将type和content组装后发送给服务器
     * @param type 消息类型
     * @param content 发送的消息，一个字符串
     * @return 成功返回true,失败返回false
     */
    bool sendMsg(int type,const std::string& content = "");

    /**
     * @brief 这是一个线程体函数，用于开辟新线程专门处理接收信息
     */
    void recvMsg();


private:
    int client_sockfd;  //客户端套接字

    std::string m_name;

    bool running;

    void errLog(const char *msg);

};

#endif