#include "neovim.hpp"

int main() {
    boost::asio::io_service io_service;
    NeoVim client(io_service);
    io_service.run();
    
    //sleep(2);

    client.send("vim_list_runtime_paths"); 

    //boost::asio::io_service::strand strand = socket.get_io_service();
    /*
    msgpack::type::tuple<int, bool, std::string> src(1, true, "example");

    std::stringstream buffer;
    msgpack::pack(buffer, src);

    buffer.seekg(0);

    std::string str(buffer.str()); //binary

    msgpack::object_handle oh = msgpack::unpack(str.data(), str.size());

    msgpack::object deserialized = oh.get();

    std::cout << deserialized << std::endl;
    
    msgpack::type::tuple<int, bool, std::string> dst;
    deserialized.convert(dst);
    */

    sleep(5);

    return 0;
}
