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
    
    socket_.write(sbuf.data(), sbuf.size(), 5);
    
    msgpack::unpacker unpacker;
    unpacker.reserve_buffer(32*1024ul);
    
    size_t rlen = socket_.read(unpacker.buffer(), unpacker.buffer_capacity(), 5);
    msgpack::unpacked result;
    unpacker.buffer_consumed(rlen);
    while(unpacker.next(result)) {
        const msgpack::object &obj = result.get();
        std::cout << "res = " << obj << std::endl;
        result.zone().reset();
    }
}

