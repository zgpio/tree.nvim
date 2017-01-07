namespace nvim {

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

template<typename T, typename...U>
void NvimRPC::send(const std::string &method, T& res, const U&...u) {
    Object v = do_send(method, u...);
    std::cout << "T NvimRPC::send" << std::endl;
    
    //if(!v.is_nil()) return boost::get<T>(v);
    //ret = boost::get<T>(v);
    //return true;

    res = boost::get<T>(v);
}

template<typename...U>
void NvimRPC::send(const std::string &method, Integer& res, const U& ...u) {
    Object v = do_send(method, u...);
    std::cout << "Integer NvimRPC::send" << std::endl;
    
    // int64_t is only for negative integer.
    if(v.is_int64_t())       res = v.as_int64_t();
    else if(v.is_uint64_t()) res = v.as_uint64_t();
    else std::cout << "invalid response type" << std::endl; //TODO: add error handler
}

template<typename...U>
void NvimRPC::send(const std::string &method, Object& res, const U& ...u) {
    Object v = do_send(method, u...);
    std::cout << "Object NvimRPC::send" << std::endl;
    
    res = v;
    //return true;
}

template<typename...U>
void NvimRPC::send(const std::string &method, nullptr_t res, const U&...u) {
    do_send(method, u...);
    std::cout << "void NvimRPC::send" << std::endl;

    //return true;
}

template<typename...U>
Object NvimRPC::do_send(const std::string &method, const U&...u) {
    msgpack::sbuffer sbuf;
    internal::Packer pk(&sbuf);
    pk.pack_array(4) << (uint64_t)REQUEST
                     << msgid_++
                     << method;
    
    pk.pack_array(sizeof...(u));
    internal::pack(pk, u...);
    
    msgpack::object_handle oh = msgpack::unpack(sbuf.data(), sbuf.size());

    msgpack::object deserialized = oh.get();

    std::cout << "sbuf = " << deserialized << std::endl;
    
    socket_.write(sbuf.data(), sbuf.size(), 5);
    
    msgpack::unpacker unpacker;
    unpacker.reserve_buffer(32*1024ul);
    
    size_t rlen = socket_.read(unpacker.buffer(), unpacker.buffer_capacity(), 5);
    msgpack::unpacked result;
    unpacker.buffer_consumed(rlen);
    
    /*
    while(unpacker.next(result)) {
        const msgpack::object &obj = result.get();
        std::cout << "res = " << obj << std::endl;
        result.zone().reset();
    }
    */
    
    //TODO: full-state response handler should be implemented
    unpacker.next(result);
    const msgpack::object &obj = result.get();
    std::cout << "res = " << obj << std::endl;
    msgpack::type::tuple<int64_t, int64_t, Object, Object> dst;
    obj.convert(dst);
    return dst.get<3>();
}

} //namespace nvim
