#ifndef NEOVIM_CPP__NVIM_RPC_HPP
#define NEOVIM_CPP__NVIM_RPC_HPP
#define BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT

#include "msgpack.hpp"
#include "util.h"
#include <string>
#include <iostream>
#include <sstream>

#include "socket.hpp"

namespace nvim {

using Integer = int64_t;
using Window = Integer;
using Buffer = Integer;
using Tabpage = Integer;
using Object = msgpack::type::variant;
// https://github.com/msgpack/msgpack-c/wiki/v2_0_cpp_variant
using Map = std::multimap<Object, Object>;
using Dictionary = Map;
using Array = std::vector<Object>;

class NvimRPC {

    enum {
        REQUEST  = 0,
        RESPONSE = 1,
        NOTIFY   = 2
    };

public:
    NvimRPC() :
        msgid_(0) {
    }

    void connect_tcp(const std::string &host,
            const std::string &service, double timeout_sec) {
        socket_.connect_tcp(host, service, timeout_sec);
    }

    void connect_pipe(const std::string &name, double timeout_sec) {
        socket_.connect_pipe(name, timeout_sec);
    }

    template<typename T, typename...U>
    void call(const std::string &method, T& res, const U&...u);

    template<typename...U>
    void call(const std::string &method, Integer& res, const U&...u);

    template<typename...U>
    void call(const std::string &method, Object& res, const U&...u);

    template<typename...U>
    void call(const std::string &method, nullptr_t res, const U&...u);
    inline bool send_response(uint64_t msgid, const Object &err, const Object &res);

    template<typename...U>
    void async_call(const std::string &method, const U&...u);
private:
    template<typename...U>
    Object do_call(const std::string &method, const U&...u);

    uint64_t msgid_;
public:
    Socket socket_;

};

} //namespace nvim

#include "impl/nvim_rpc.hpp"

#endif //NEOVIM_CPP__NVIM_RPC_HPP

