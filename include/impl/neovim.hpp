template<typename...T>
void NeoVim::send(const std::string &method, const T&...t) {
    msgpack::sbuffer sbuf;
    detail::Packer pk(&sbuf);
    pk.pack_array(4) << (uint64_t)REQUEST
                     << msgid_++
                     << method;
    
    pk.pack_array(sizeof...(t));
    detail::pack(pk, t...);
    
    msgpack::object_handle oh = msgpack::unpack(sbuf.data(), sbuf.size());

    msgpack::object deserialized = oh.get();

    std::cout << "sbuf = " << deserialized << std::endl;

    boost::asio::spawn(io_service_, [this, &sbuf](boost::asio::yield_context yield_ctx){
        boost::system::error_code ec;
            
        boost::asio::async_write(socket_,
            boost::asio::buffer(std::string(sbuf.data(), sbuf.size())), yield_ctx[ec]);
        
        if(ec) {
            std::cout << "send failed: " << ec.message() << std::endl;
            return;
        } 

        msgpack::unpacker unpacker;
        unpacker.reserve_buffer(32*1024ul);
        size_t size = async_read(socket_, boost::asio::buffer(unpacker.buffer(), unpacker.buffer_capacity()),
            boost::asio::transfer_at_least(1), yield_ctx[ec]);
        
        if(ec) {
            std::cout << "read failed: " << ec.message() << std::endl;
            return;
        } 

        msgpack::unpacked result;
        unpacker.buffer_consumed(size);
        while(unpacker.next(result)) {
            const msgpack::object &obj = result.get();
            std::cout << "res = " << obj << std::endl;
            result.zone().reset();
        }
    });
}

