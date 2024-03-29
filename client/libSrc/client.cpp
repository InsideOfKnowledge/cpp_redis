#include "client.h"
#include "connection.h"
#include "RESPEncoder.h"
#include "RESPDecoder.h"

#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace zz_redis {
class Client::pImpl_ {
public:
    pImpl_(std::unique_ptr<Connection>& conn) 
        : conn_(std::move(conn)) {

    }

    ~pImpl_() {

    }

    bool start() {
        // bool ret = 因为clientFd设置为非阻塞，所以无需判断返回结果
        // 连接服务端
        conn_->init();

        while (true) {
            std::cout << SERVER_ADDR << ":" << SERVER_ADDR << ">";
            cmdBuf.clear();
            std::getline(std::cin, cmdBuf);

            // 解析用户输入，组装成RESP协议格式
            std::string outbuf = RESPEncoder::strToArray(cmdBuf);

            // 发送到服务端
            conn_->write(outbuf);

            // epoll_wait，等待服务端响应
            // 使用epoll_wait的好处是避免客户端程序一直占用cpu资源
            size_t n = conn_->epoll();
            if (n == -1) 
                break; // 出错
            // 只绑定了一个clientFd，因此可读、可写事件一定都是该客户端的
            // 处理可读事件
            // 将数据读到conn的接收缓冲区中
            conn_->read();
            this->outBuf = conn_->getRecv();
            // 对回复进行解析
            std::string res = RESPDecoder::decode(this->outBuf);
            std::cout << res << std::endl;
        }


        return true;
    }

    int getFd() {
        return conn_->getFd();
    }

private:
    std::string                 cmdBuf;     // 客户端命令行缓冲区
    std::string                 outBuf;     // 服务端的响应
    std::unique_ptr<Connection> conn_;
};

Client::Client(std::unique_ptr<Connection>& conn) 
    : sp_pImpl_(std::make_unique<pImpl_>(conn)) {
    
}

Client::~Client() {

}

bool Client::start() {
    // 开始
    sp_pImpl_->start();

    return true;
}

void Client::printInfo() {
    std::cout << "客户端套接字描述符 : " << sp_pImpl_->getFd() << std::endl;
}
}