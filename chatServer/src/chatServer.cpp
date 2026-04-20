#include "../header/chatServer.h"

ChatServer::ChatServer(const char *ip, int port, int threadPoolSize)
{
    this->server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->server_sockfd == -1)
    {
        perror("socket error");
        return;
    }

    // 端口号快速重用
    int opt = 1;
    if (setsockopt(this->server_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt error");
        return;
    }

    // 填充地址信息结构体
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // 绑定地址
    if (bind(this->server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind error");
        return;
    }

    // 启动监听
    if (listen(this->server_sockfd, 10) < 0)
    {
        perror("listen error");
        return;
    }

    // 启动线程池
    this->threadPool = new whThreadPool(threadPoolSize);
}

ChatServer::~ChatServer()
{
    // 关闭服务器套接字
    close(this->server_sockfd);

    // 释放线程池资源
    delete this->threadPool;
}

void ChatServer::run()
{
    // 循环等待客户端连接
    while (true)
    {
        struct sockaddr_in cin; // 用于接收客户端地址信息
        socklen_t clen = sizeof(cin);

        int client_sockfd = accept(this->server_sockfd, (struct sockaddr *)&cin, &clen);
        if (client_sockfd == -1)
        {
            errLog("accept error");
            continue;
        }
        std::cout << "new connect!\n";
        // 将处理客户端的任务添加到线程池中
        this->threadPool->addTask([this, client_sockfd, cin]
                                  { this->handleClient(client_sockfd, cin); });
    }
}

void ChatServer::handleClient(int client_sockfd, struct sockaddr_in client_addr)
{
    MSG msg;
    char buffer[1024];
    while (true)
    {
        memset(buffer,0,sizeof(buffer));
        // 读取客户端
        int recv_len = recv(client_sockfd, buffer, sizeof(buffer), 0);
        if (recv_len <= 0)
        {
            // 从在线列表中删除该客户端
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.erase(client_sockfd);
            close(client_sockfd);
            return;
        }
        msg.deserialize(std::string(buffer, recv_len));
        // 对消息类型进行判断
        switch (ntohl(msg.type))
        {
        case LOGIN: // 登录消息
        {
            std::unique_lock<std::mutex> lock(clients_mutex);
            Client new_client;
            new_client.sockfd = client_sockfd;
            new_client.addr = client_addr;
            clients[client_sockfd] = new_client;
            lock.unlock();
            //广播消息给所有客户端
            msg.content = std::string("------") + msg.name + "上线了" + "-------";
            broadcast(msg);
            break;
        }
        case CHAT: // 聊天消息
        {
            std::unique_lock<std::mutex> lock(clients_mutex);
            broadcast(msg, client_sockfd);
            lock.unlock();
            break;
        }
        case QUIT: // 退出消息
        {
            std::unique_lock<std::mutex> lock(clients_mutex);
            clients.erase(client_sockfd);
            lock.unlock();
            msg.content = std::string("------") + msg.name + "下线了" + "-------";
            broadcast(msg, client_sockfd);
            close(client_sockfd);
            break;
        }
        default:
            break;
        }
    }
}

void ChatServer::broadcast(const MSG& msg, int exclude_sockfd)
{
    std::string data = msg.serialize();
    std::cout << "有广播，数据为：" << data << '\n';
    for(const auto& [sockfd, client] : clients)
    {
        if(sockfd != exclude_sockfd)
        {
            if(send(sockfd, data.c_str(), data.size(), 0) < 0)
            {
                errLog("send error");
                return;
            }
        }
    }
}

void ChatServer::errLog(const char *msg)
{
    std::cerr << __FILE__ << " " << __func__ << " " << __FILE__ << std::endl;
    perror(msg);
}