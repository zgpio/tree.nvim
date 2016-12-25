#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/system/system_error.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

using boost::asio::deadline_timer;
using boost::asio::ip::tcp;
using boost::lambda::bind;
using boost::lambda::var;
using boost::lambda::_1;
using boost::lambda::_2;

class Socket
{
public:
  Socket()
    : socket_(io_service_),
      deadline_(io_service_)
  {
    deadline_.expires_at(boost::posix_time::pos_infin);
    check_deadline();
  }

  void connect(const std::string& host, 
               const std::string& service, double timeout_sec);
  
  size_t read(char *rbuf, size_t capacity, double timeout_sec);
  void write(char *sbuf, size_t size, double timeout_sec);

private:
  void check_deadline();
  
  boost::asio::io_service io_service_;
  tcp::socket socket_;
  deadline_timer deadline_;
  boost::asio::streambuf input_buffer_;
};

#endif //SOCKET_HPP_
