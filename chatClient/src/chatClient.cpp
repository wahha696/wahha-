#include "../header/chatClient.h"

ChatClient::ChatClient(const char *ip, int port, const std::string &name) : m_name(name), running(true)
{
    this->client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->client_sockfd < 0)
    {
        perror("socket error");
        return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);
    // 连接服务器
    if (connect(client_sockfd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect error");
        return;
    }
    this->sendMsg(LOGIN);
}

ChatClient::~ChatClient()
{
    this->sendMsg(QUIT);
    close(this->client_sockfd);
}

void ChatClient::run()
{
    std::thread rec([this]
                    { this->recvMsg(); });

    while (running)
    {
        std::string tmp;
        std::getline(std::cin, tmp);
        if (tmp.find('|') != std::string::npos)
        {
            std::cout << "[Info]输入内容不能有'|'，请重新输入\n";
            continue;
        }
        if (!this->sendMsg(CHAT, tmp))
        {
            std::cout << "[Info]发送失败！\n";
        }
        else
        {
            std::cout << "[Info]发送成功！\n";
        }
    }

    rec.join();
}

bool ChatClient::sendMsg(int type, const std::string &content)
{
    MSG msg;
    msg.type = htonl(type);
    msg.content = content;
    msg.name = this->m_name;
    std::string tmp = msg.serialize();
    if (send(this->client_sockfd, tmp.c_str(), tmp.size(), 0) < 0)
    {
        perror("send error");
        return false;
    }
    if(type == QUIT)
    {
        running = false;
    }

    return true;
}

void ChatClient::recvMsg()
{
    char buf[2048];
    while(running)
    {
        memset(buf,0,sizeof(buf));
        int recv_len = recv(this->client_sockfd,buf,sizeof(buf),0);
        std::cout << "收到消息！recv_len = " << recv_len << '\n';
        if(recv_len < 0)
        {
            if(errno == EAGAIN) //try again
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }else
            {
                errLog("recv error");
                running = false;
                return;
            }
        }
        else if(recv_len == 0)
        {
            errLog("服务器下线");
            return;
        }
        else
        {
            MSG msg;
            msg.deserialize(std::string(buf,recv_len));
            std::cout << msg.name << ":" << msg.content << std::endl;
        }
    }
}

void ChatClient::errLog(const char *msg)
{
    std::cerr << __FILE__ << " " << __func__ << " " << __FILE__ << std::endl;
    perror(msg);
}