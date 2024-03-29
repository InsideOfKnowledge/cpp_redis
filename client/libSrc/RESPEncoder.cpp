#include "RESPEncoder.h"
#include <sstream>

namespace zz_redis {
std::string RESPEncoder::strToArray(const std::string& input) {
    std::vector<std::string> tokens = splitString(input, ' ');
    size_t len = tokens.size();

    std::string outbuf;
    // cmd:         set key value
    // RESP格式示例：*3\r\n$3\r\nset\r\n$3\r\nkey\r\n$5\r\nvalue\r\n
    outbuf.append("*");
    outbuf.append(std::to_string(len));
    outbuf.append("\r\n");

    for (int i = 0; i < len; ++i) {
        size_t s = tokens[i].length();
        outbuf.append("$");
        outbuf.append(std::to_string(s));
        outbuf.append("\r\n");
        outbuf.append(tokens[i]);
        outbuf.append("\r\n");
    }

    return outbuf;
}

std::vector<std::string> RESPEncoder::splitString(const std::string& input, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(input);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}


} // end of namaspace zz_redis