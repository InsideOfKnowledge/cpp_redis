#ifndef __connection_h__
#define __connection_h__

#include <memory>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 12345
#define SEND_BUF_MAX_SIZE 512 * 1024 * 1024

namespace zz_redis {
class Connection final {
public:
    Connection();
    ~Connection();

    bool init();
    int getFd();
    bool write(const std::string& out);
    int read();
    size_t epoll();
    std::string getRecv();

private:
    class                   pImpl_;
    std::unique_ptr<pImpl_> sp_pImpl_;
};
} // end of namespace zz_redis

#endif