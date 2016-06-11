#include <msgpack.hpp>
#include <string>
#include <iostream>
#include <sstream>

//#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/yield.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/write.hpp>

int main() {
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::socket socket(io_service);

    //boost::asio::io_service::strand strand = socket.get_io_service();

    boost::asio::spawn(io_service, [](boost::asio::yield_context yield){
        //async_read(socket, asio::buffer(buffer_), yield);
        //async_write(socket, asio::buffer(buffer_), yield);

        //do something...
    });

    msgpack::type::tuple<int, bool, std::string> src(1, true, "example");

    std::stringstream buffer;
    msgpack::pack(buffer, src);

    buffer.seekg(0);

    std::string str(buffer.str());

    msgpack::object_handle oh = msgpack::unpack(str.data(), str.size());

    msgpack::object deserialized = oh.get();

    std::cout << deserialized << std::endl;
    
    msgpack::type::tuple<int, bool, std::string> dst;
    deserialized.convert(dst);

    return 0;
}
