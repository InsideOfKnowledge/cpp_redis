#ifndef __RESPEncoder_h__
#define __RESPEncoder_h__

#include <vector>
#include <string>

namespace zz_redis {
class RESPEncoder final {
public:
    static std::string strToArray(const std::string& input);

private:
    static std::vector<std::string> splitString(const std::string& input, char delimiter);
};
} // end of namaspace zz_redis

#endif