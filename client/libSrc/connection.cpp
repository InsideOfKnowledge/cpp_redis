#include "connection.h"

#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>

namespace zz_redis {
class Connection::pImpl_ {
public:
    pImpl_() : clientFd(-1) {
        epfd = epoll_create(1);
    }
    ~pImpl_() {
        if (clientFd != -1)
            close(clientFd);
    }

    // 初始化连接
    bool init() {
        if (clientFd != -1)
            return true;

        // 设置客户端为非阻塞套接字
        clientFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
        if (clientFd == -1)
            return false;

        // 绑定到epfd上
        createFileEvent();

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
        serverAddr.sin_port = htons(SERVER_PORT);

        int ret = -1;
        while (true) {
            ret = connect(clientFd, (sockaddr*)&serverAddr, sizeof(serverAddr));
            if (ret == -1) {
                // 防止被信号中断，重新调用connect
                if (errno == EWOULDBLOCK)
                    break;
                if (errno == EINTR)
                    continue;

                return false;
            }
            break;
        }

        return true;
    }

    void createFileEvent() {
        epoll_event event;
        event.data.fd = clientFd;
        // 只注册监听可读事件
        event.events = EPOLLIN;
        epoll_ctl(epfd, EPOLL_CTL_ADD, clientFd, &event);
    }

    int getFd() {
        return clientFd;
    }

    bool write(const std::string& out) {
        if (sendBuf.size() != 0) {
            // 发送缓冲区长度检测，最长为512M
            if (out.size() + sendBuf.size() < SEND_BUF_MAX_SIZE) { 
                sendBuf.append(out);
                return true;
            } else {
                return false;
            }
        }

        // 直接发送
        const char* b = out.c_str();
        size_t len = out.size();
        int ret = -1;
        while (true) {
            ret = send(clientFd, b, len, 0);
            if (ret == -1) {
                if (errno == EWOULDBLOCK) {
                    // 将剩余的数据保存到发送缓冲区
                    sendBuf.append(b, len);
                }
                if (errno == EINTR)
                    continue;
                return false;
            }
            if (ret != out.size()) {
                // 将剩余的数据保存到发送缓冲区
                sendBuf.append(b + ret, len - ret);
            }
            break;
        }

        return true;
    }

    int read() {
        char crecvbuf[1024] {0};
        int n = -1;
        int total = 0;
        while (true) {
            // 一次性读完
            n = recv(clientFd, crecvbuf, 1024, 0);
            total += n;
            if (n > 0)
                recvBuf.append(crecvbuf, n);
            if (n == -1) {
                if (errno == EWOULDBLOCK)
                    // 读完了
                    break;
                if (errno == EINTR)
                    continue;
                return -1;
            }
        }

        return total;
    }

    size_t epoll() {
        int n = -1;
        while (true) {
            n = epoll_wait(epfd, events, 1, 1000);
            if (n == -1) {
                // 如果被信号中断，重新执行
                if (errno == EINTR)
                    continue;
                break;
            }
            if (n == 0)
                // 超时，继续epoll_wait，直到发生了可读事件
                continue;
            break;
        }

        return n;
    }

    std::string getRecv() {
        return recvBuf;
    }

private:
    int         clientFd;       // 客户端fd
    std::string sendBuf;        // 发送缓冲区
    std::string recvBuf;        // 接收缓冲区
    int         epfd;           // 多路转接监听fd
    epoll_event events[1];      // 返回事件数组
};

Connection::Connection() : sp_pImpl_(std::make_unique<pImpl_>()) {

}

Connection::~Connection() {

}

bool Connection::init() {
    return sp_pImpl_->init();
}

int Connection::getFd() {
    return sp_pImpl_->getFd();
}

bool Connection::write(const std::string& out) {
    return sp_pImpl_->write(out);
}

int Connection::read() {
    return sp_pImpl_->read();
}

size_t Connection::epoll() {
    return sp_pImpl_->epoll();
}

std::string Connection::getRecv() {
    return sp_pImpl_->getRecv();
}
} // end of namespace zz_redis