#ifndef NEOVIM_CPP__SOCKET_HPP
#define NEOVIM_CPP__SOCKET_HPP
#define BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/generic/stream_protocol.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/system/system_error.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

#include <msgpack.hpp>
// #include <msgpack.h>
namespace nvim {

//TODO Implement asynchronous response handler.
class Socket {
public:
    Socket() : 
        socket_(io_service_),
        deadline_(io_service_)
    {
        deadline_.expires_at(boost::posix_time::pos_infin);
        check_deadline();
    }

    void connect_tcp(const std::string& host, 
               const std::string& service, double timeout_sec);
    void connect_pipe(const std::string& name, double timeout_sec);

    size_t read(char *rbuf, size_t capacity, double timeout_sec);
    msgpack::unpacked read2(double timeout_sec);
    void write(char *sbuf, size_t size, double timeout_sec);

private:
    void check_deadline();

    boost::asio::io_service io_service_;
    boost::asio::generic::stream_protocol::socket socket_;
    boost::asio::deadline_timer deadline_;
    boost::asio::streambuf input_buffer_;
};

} //namespace nvim

#endif //NEOVIM_CPP__SOCKET_HPP
