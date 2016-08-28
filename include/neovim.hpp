#ifndef NEOVIM_HPP_
#define NEOVIM_HPP_

#include <msgpack.hpp>
#include <string>
#include <iostream>
#include <sstream>

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/yield.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>

class NeoVim {
    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::socket socket_;
    uint64_t msgid_;
    
    enum {
        REQUEST  = 0,
        RESPONSE = 1,
        NOTIFY   = 2
    };
    
public:
    NeoVim()
        : io_service_(),
          socket_(io_service_),
          msgid_(0)
    {
        connect();
    }

    template<typename...T>
    void send(const std::string &method, const T&...t);
    
private:
    void connect();

};

#include "impl/neovim.hpp"

#endif //NEOVIM_HPP_
