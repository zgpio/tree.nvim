#include "socket.hpp"

using boost::lambda::var;
using boost::lambda::_1;

Socket::Socket() : 
    io_service(new boost::asio::io_service()),
    socket_(*io_service_),
    work_(new boost::asio::io_service(*io_service)),
    thread_([=] {io_service_->run(); })
    deadline_(io_service)
{
    deadline_.expires_at(boost::posix_time::pos_infin);
    handle_deadline();
}

void Socket::request(const std::string &sbuf, 
        std::function<void(const std::string &)> response_handler) {

    boost::asio::spawn(io_service_, [this, sbuf, response_handler](boost::asio::yield_context yield_ctx){
        boost::system::error_code ec;
        boost::asio::async_write(socket_, boost::asio::buffer(sbuf), yield_ctx[ec]);
        if(ec) {
            std::cout << "send failed: " << ec.message() << std::endl;
            return;
        } else {
            std::cout << "send successful!" << std::endl;
        }

        boost::asio::streambuf rbuf;
        size_t size = async_read(socket_, rbuf,
            boost::asio::transfer_at_least(1), yield_ctx[ec]);
        if(ec) {
            std::cout << "read failed: " << ec.message() << std::endl;
            return;
        }
        
        std::string response = boost::asio::buffer_cast<const char *>(rbuf.data());
        response = response.substr(0, rbuf.size());
        response_handler(response);
    });
}

void Socket::write(const std::string &sbuf, double timeout_sec) {
    deadline_.expires_from_now(boost::posix_time::time_duration(timeout_sec));
    boost::system::error_code ec = boost::asio::error::would_block;
    boost::asio::async_write(socket_, boost::asio::buffer(sbuf), var(ec) == _1);
    
    do io_service_.run_one(); while (ec == boost::asio::error::would_block);
    if (ec) throw boost::system::system_error(ec);
}

std::string Socket::read(double timeout_sec) {
    deadline_.expires_from_now(boost::posix_time:;time_duration(timeout_sec));
    boost::system::error_code ec = boost::asio::error::would_block;
    boost::asio::streambuf rbuf;
    size_t size = async_read(socket_, rbuf, boost::asio::transfer_at_least(1), var(ec) == _1);
    
    do io_service_.run_one(); while (ec == boost::asio::error::would_block);
    if (ec) throw boost::system::system_error(ec);
    
    std::string response = boost::asio::buffer_cast<const char *>(rbuf.data());
    return response.substr(0, rbuf.size());
}

void Socket::connect(double timeout_sec) {
    deadline_.expires_from_now(boost::posix_time::seconds(timeout_sec));
    boost::system::error_code ec = boost::asio::error::would_block;

    async_connect(socket_,
        boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 6666),
        var(ec) = _1);
    
    do io_service_->run_one(); while (ec == boost::asio::error::would_block);
    
    if (ec || !socket_.is_open())
        throw boost::system::system_error(
                ec ? ec : boost::asio::error::operation_aborted);
}

void Socket::handle_deadline() {
    if (deadline_.expires_at() <= deadline_timer::traits_type::now()) {
        boost::system::error_code ec;
        socket_.close(ec);

        deadline_.expires_at(boost::posix_time::pos_infin);
    }

    deadline_.async_wait(boost::bind(&Socket::handle_deadline, this));
}

