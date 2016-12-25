#ifndef NEOVIM_HPP_
#define NEOVIM_HPP_

#include <msgpack.hpp>
#include <string>
#include <iostream>
#include <sstream>

/*
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <atomic>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>
*/

#include "socket.hpp"

class NeoVim {
    enum {
        REQUEST  = 0,
        RESPONSE = 1,
        NOTIFY   = 2
    };
    
public:
    NeoVim() : 
        msgid_(0) {
        socket_.connect("127.0.0.1", "6666", 1.0);
    }

    template<typename...T>
    void send(const std::string &method, const T&...t);
    
private:
    uint64_t msgid_;
    Socket socket_;

};

#include "impl/neovim.hpp"

#endif //NEOVIM_HPP_

