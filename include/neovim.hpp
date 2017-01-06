#ifndef NEOVIM_HPP_
#define NEOVIM_HPP_

#include "msgpack.hpp"
#include <string>
#include <iostream>
#include <sstream>

#include "socket.hpp"

//TODO: wrap as namespace;
using Integer = int64_t;
using Window = Integer;
using Buffer = Integer;
using Tabpage = Integer;
using Object = msgpack::type::variant;

class NvimRPC {
        
    enum {
        REQUEST  = 0,
        RESPONSE = 1,
        NOTIFY   = 2
    };
    
public:
    NvimRPC() : 
        msgid_(0) {
        socket_.connect("127.0.0.1", "6666", 1.0);
    }

    template<typename T, typename...U>
    bool send(const std::string &method, T& ret, const U&...u);
    
    template<typename...U>
    bool send(const std::string &method, Integer& ret, const U&...u);

    template<typename...U>
    bool send(const std::string &method, Object& ret, const U&...u);

    template<typename...U>
    bool send(const std::string &method, const U&...u);
    
private:
    template<typename...U>
    Object do_send(const std::string &method, const U&...u);
    
    uint64_t msgid_;
    Socket socket_;

};

#include "impl/neovim.hpp"

#endif //NEOVIM_HPP_

