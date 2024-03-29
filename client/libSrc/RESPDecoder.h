#ifndef __RESPDncoder_h__
#define __RESPDncoder_h__

#include <string>

namespace zz_redis {
struct BulkString {
};

class RESPDecoder final {
public:
    static std::string decode(const std::string& input);
    static std::string _decode(const std::string& input, BulkString);
};


} // end of namespace zz_redis

#endif