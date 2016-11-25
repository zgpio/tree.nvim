#ifndef NEOVIM_HPP_
#define NEOVIM_HPP_

#include <msgpack.hpp>
#include <string>
#include <iostream>
#include <sstream>

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
//#include <boost/asio/yield.hpp>
#include <atomic>
#include <thread>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>

#include <thread>

class NeoVim {
    enum {
        REQUEST  = 0,
        RESPONSE = 1,
        NOTIFY   = 2
    };
    
public:
    NeoVim()
        : io_service_(new boost::asio::io_service()),
          socket_(*io_service_),
          work_(new boost::asio::io_service::work(*io_service_)),
          thread_([=] { io_service_->run(); }),
          msgid_(0)
    {
        
        connect();
    }

    template<typename...T>
    void send(const std::string &method, const T&...t);
    
private:
    void connect();
    
    std::shared_ptr<boost::asio::io_service> io_service_;
    boost::asio::ip::tcp::socket socket_;
    std::unique_ptr<boost::asio::io_service::work> work_;
    std::thread thread_;
    uint64_t msgid_;

};

#include "impl/neovim.hpp"

#endif //NEOVIM_HPP_

