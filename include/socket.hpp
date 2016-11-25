#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include <boost/asio/connect.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/yield.hpp>

class Socket {
public:
    Socket();

    void connect(double timeout_sec = 1.0);
    void request(const std::string &req,
            std::function<void(const std::string &)> response_handler);

    std::string read(double timeout_sec = 1.0);
    void write(const std::string &sbuf, double timeout_sec = 1.0);

private:
    void handle_deadline();
    
    std::shared_ptr<boost::asio::io_service> io_service_;
    std::unique_ptr<boost::asio::io_service::work> work_;
    std::thread thread_;
    boost::asio::tcp::socket socket_;
    boost::asio::deadline_timer deadline_;

};

#endif //SOCKET_HPP_
