#include "RESPDecoder.h"

namespace zz_redis {
std::string RESPDecoder::decode(const std::string& input) {
    char c = input[0];
    if (c == '$')
        return _decode(input.substr(1, input.size()-1), BulkString());
    
    return "";
}

std::string RESPDecoder::_decode(const std::string& input, BulkString) {
    size_t index = input.find("\\r");
    size_t num = 0;
    for (size_t i = 0; i < index; ++i) {
        num = num * 10 + (input[i] - '0');
    }

    return input.substr(index + 4, 5); 
}

} // end of namespace zz_redis