#include "socket.hpp"

#include <boost/lambda/bind.hpp>
// #include <boost/lambda/lambda.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/ip/tcp.hpp>

#define BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT

using boost::asio::deadline_timer;
using boost::asio::ip::tcp;
using boost::lambda::bind;
// using boost::lambda::var;
// using boost::lambda::_1;
// using boost::lambda::_2;

namespace nvim {

void Socket::connect_tcp(const std::string& host,
                     const std::string& service, double timeout_sec)
{
    tcp::resolver::query query(host, service);
    tcp::resolver::iterator iter = tcp::resolver(io_service_).resolve(query);

    deadline_.expires_from_now(boost::posix_time::seconds(long(timeout_sec)));

    boost::system::error_code ec;

    for (; iter != tcp::resolver::iterator(); ++iter) {
        socket_.close();
        ec = boost::asio::error::would_block;
        // socket_.async_connect(iter->endpoint(), var(ec) = _1);
        socket_.async_connect(iter->endpoint(),
                        [&ec](boost::system::error_code e){
                            ec = e;
                        });

        do io_service_.run_one(); while (ec == boost::asio::error::would_block);
        if (!ec && socket_.is_open()) return;
    }

    throw boost::system::system_error(
        ec ? ec : boost::asio::error::host_not_found);
}

size_t Socket::read(char *rbuf, size_t capacity, double timeout_sec) {
    deadline_.expires_from_now(boost::posix_time::seconds(long(timeout_sec)));
    boost::system::error_code ec = boost::asio::error::would_block;
    size_t rlen;
    async_read(socket_, boost::asio::buffer(rbuf, capacity),
        boost::asio::transfer_at_least(1),
        [&ec, &rlen](boost::system::error_code e, size_t s) {ec = e; rlen = s;});

    // Block until the asynchronous operation has completed.
    do io_service_.run_one(); while (ec == boost::asio::error::would_block);
    if (ec) throw boost::system::system_error(ec);

    return rlen;
}
/// 能够根据msgpack message的长度, 动态增加缓冲区
msgpack::unpacked Socket::read2(double timeout_sec)
{
    size_t rlen = 0;
    // msgpack::unpacker unp;
    msgpack::unpacker unp(
            [](msgpack::type::object_type /*type*/, std::size_t /*len*/, void*) -> bool { return true; },
            MSGPACK_NULLPTR,
            32
            );

    do {
        deadline_.expires_from_now(boost::posix_time::seconds(long(timeout_sec)));
        boost::system::error_code ec = boost::asio::error::would_block;
        if (unp.buffer_capacity() == 0) {
            std::cout << "-----reserve_buffer-----" << std::endl;
            unp.reserve_buffer(8192);
        }

        rlen = socket_.read_some(boost::asio::buffer(unp.buffer(), unp.buffer_capacity()));
        // TODO: 异步读取超时导致套接字关闭
        // async_read(
        //     socket_,
        //     boost::asio::buffer(unp.buffer(), unp.buffer_capacity()),
        //     boost::asio::transfer_at_least(1),
        //     [&ec, &rlen](boost::system::error_code e, size_t s) {
        //         ec = e;
        //         rlen = s;
        //     });
        //
        // do io_service_.run_one(); while (ec == boost::asio::error::would_block);
        // if (ec) throw boost::system::system_error(ec);

        if (rlen > 0) {
            msgpack::unpacked result;
            unp.buffer_consumed(rlen);
            if (unp.next(result)) {
                return result;
            }
        }
    } while (rlen > 0);
}

void Socket::write(char *sbuf, size_t size, double timeout_sec) {
    deadline_.expires_from_now(boost::posix_time::seconds(long(timeout_sec)));
    boost::system::error_code ec = boost::asio::error::would_block;
    // boost::asio::async_write(socket_, boost::asio::buffer(sbuf, size), var(ec) = _1);
    boost::asio::async_write(socket_, boost::asio::buffer(sbuf, size),
                [&ec](boost::system::error_code e, size_t s){
                    ec = e;
                });

    do io_service_.run_one(); while (ec == boost::asio::error::would_block);

    if (ec) throw boost::system::system_error(ec);
}

void Socket::check_deadline() {
    if (deadline_.expires_at() <= deadline_timer::traits_type::now()) {
        socket_.close();
        deadline_.expires_at(boost::posix_time::pos_infin);
    }

    deadline_.async_wait(bind(&Socket::check_deadline, this));
}

} //namespace nvim

