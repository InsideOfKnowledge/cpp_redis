#ifndef __client_h__
#define __client_h__

#include <memory>

namespace zz_redis {
class Connection;

class Client final {
public:
    Client(std::unique_ptr<Connection>& conn);
    ~Client();
    Client(const Client& cl) = delete;
    Client& operator= (const Client& cl) = delete;

    bool start();

    void printInfo();

private:
    class                   pImpl_;
    std::unique_ptr<pImpl_> sp_pImpl_;
};
}

#endif