#ifndef NVIM_CPP_UTIL
#define NVIM_CPP_UTIL
#include <string>
#include "msgpack.hpp"
std::string type_name(const msgpack::type::variant &obj);
std::string gbk_to_utf8(const std::string& str);
extern bool IS_GBK;
#endif
