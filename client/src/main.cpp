#include "../libSrc/client.h"
#include "../libSrc/connection.h"

using namespace zz_redis;

int main() {

    std::unique_ptr<Connection> sp_conn = std::make_unique<Connection>();

    Client client(sp_conn);

    client.start();

    return 0;
}