namespace internal {
    using Packer = msgpack::packer<msgpack::sbuffer>;

    template<class X>
    Packer& pack(Packer& pk, const X& x) {
          return pk << x;
    }
    
    template<class X, class Y, class...Z>
    Packer& pack(Packer& pk, const X &x, const Y &y, const Z &...z) {
          return pack(pack(pk, x), y, z...);
    }

    static Packer& pack(Packer& pk) {
          return pk;
    }
} // namespace internal

template<typename...T>
void NeoVim::send(const std::string &method, const T&...t) {
    msgpack::sbuffer sbuf;
    internal::Packer pk(&sbuf);
    pk.pack_array(4) << (uint64_t)REQUEST
                     << msgid_++
                     << method;
    
    pk.pack_array(sizeof...(t));
    internal::pack(pk, t...);
    
    msgpack::object_handle oh = msgpack::unpack(sbuf.data(), sbuf.size());

    msgpack::object deserialized = oh.get();

    std::cout << "sbuf = " << deserialized << std::endl;
    
    boost::system::error_code ec;
            
    boost::asio::write(socket_, 
        boost::asio::buffer(sbuf.data(), sbuf.size()));
    
    if(ec) {
        std::cout << "send failed: " << ec.message() << std::endl;
        return;
    } else {
        std::cout << "send successful!" << std::endl;
    }

    msgpack::unpacker unpacker;
    unpacker.reserve_buffer(32*1024ul);
    size_t size = read(socket_, boost::asio::buffer(unpacker.buffer(), unpacker.buffer_capacity()),
        boost::asio::transfer_at_least(1));
    
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


    /*
    ///< @TODO: check &sbuf is available for this.
    boost::asio::spawn(*io_service_, [this, method, t](boost::asio::yield_context yield_ctx){
        
        boost::system::error_code ec;
            
        boost::asio::async_write(socket_, 
            boost::asio::buffer(sbuf.data(), sbuf.size()), yield_ctx[ec]);
        
        if(ec) {
            std::cout << "send failed: " << ec.message() << std::endl;
            return;
        } else {
            std::cout << "send successful!" << std::endl;
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
    */
}

void NeoVim::connect() {
    socket_.async_connect(
        boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 6666),
        [this](const boost::system::error_code &ec) {
            if(ec) {
                std::cout << "connect failed: " << ec.message() << std::endl;
            } else {   
                std::cout << "connected" << std::endl;
                send("vim_del_current_line");
                send("vim_list_runtime_paths");
            }
        }
    );
}

