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

namespace detail {
    using Packer = msgpack::packer<msgpack::sbuffer>;

    template<class X>
        Packer& pack(Packer& pk, const X& x)
        {
              return pk << x;
        }
    template<class X, class Y, class...Z>
        Packer& pack(Packer& pk, const X &x, const Y &y, const Z &...z)
        {
              return pack(pack(pk, x), y, z...);
        }


    static Packer& pack(Packer& pk)
    {
          return pk;
    }
} // namespace detail

class NeoVim {
    boost::asio::io_service &io_service_;
    boost::asio::ip::tcp::socket socket_;
    uint64_t msgid_;
    
    enum {
        REQUEST  = 0,
        RESPONSE = 1,
        NOTIFY   = 2
    };
    
public:
    NeoVim(boost::asio::io_service &io_service)
        : io_service_(io_service),
          socket_(io_service),
          msgid_(0)
    {
        connect();
    }

    template<typename...T>
    void send(const std::string &method, const T&...t);
    
private:
    void connect() {
        socket_.async_connect(
            boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 6666),
            [this](const boost::system::error_code &ec) {
                if(ec) {
                    std::cout << "connect failed: " << ec.message() << std::endl;
                } else {   
                    std::cout << "connected" << std::endl;
                    send("vim_list_runtime_paths");
                }
            }
        );
    }
};

#include "impl/neovim.hpp"

#endif //NEOVIM_HPP_
