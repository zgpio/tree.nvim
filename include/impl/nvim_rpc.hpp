namespace nvim {

namespace detail {
    using Packer = msgpack::packer<msgpack::sbuffer>;

    template<class T>
    Packer& pack(Packer& pk, const T& t) {
          return pk << t;
    }
    
    template<class T1, class T2, class...T3>
    Packer& pack(Packer& pk, const T1 &t1, const T2 &t2, const T3 &...t3) {
          return pack(pack(pk, t1), t2, t3...);
    }

    static Packer& pack(Packer& pk) {
          return pk;
    }

} // namespace detail

template<typename T, typename...U>
void NvimRPC::call(const std::string &method, T& res, const U&...u) {
    Object v = do_call(method, u...);
    std::cout << "T NvimRPC::call" << std::endl;
    
    res = boost::get<T>(v);
}

template<typename...U>
void NvimRPC::call(const std::string &method, Integer& res, const U& ...u) {
    Object v = do_call(method, u...);
    std::cout << "Integer NvimRPC::call" << std::endl;
    
    // int64_t is only for negative integer.
    if(v.is_int64_t())       res = v.as_int64_t();
    else if(v.is_uint64_t()) res = v.as_uint64_t();
    else std::cout << "invalid response type" << std::endl; //TODO: add error handler
}

template<typename...U>
void NvimRPC::call(const std::string &method, Object& res, const U& ...u) {
    Object v = do_call(method, u...);
    std::cout << "Object NvimRPC::call" << std::endl;
    res = v;
}

template<typename...U>
void NvimRPC::call(const std::string &method, nullptr_t res, const U&...u) {
    do_call(method, u...);
    std::cout << "void NvimRPC::call" << std::endl;
}

template<typename...U>
Object NvimRPC::do_call(const std::string &method, const U&...u) {
    msgpack::sbuffer sbuf;
    detail::Packer pk(&sbuf);
    pk.pack_array(4) << (uint64_t)REQUEST
                     << msgid_++
                     << method;
    
    pk.pack_array(sizeof...(u));
    detail::pack(pk, u...);
    
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
