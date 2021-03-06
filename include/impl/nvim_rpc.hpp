#include "msgpack.hpp"

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
    // std::cout << "T NvimRPC::call" << std::endl;

    res = boost::get<T>(v);
}

template<typename...U>
void NvimRPC::call(const std::string &method, Integer& res, const U& ...u) {
    Object v = do_call(method, u...);
    // std::cout << "Integer NvimRPC::call" << std::endl;

    // int64_t is only for negative integer.
    if(v.is_int64_t())       res = v.as_int64_t();
    else if(v.is_uint64_t()) res = v.as_uint64_t();
    else if (v.is_ext()) {
        // NOTE: Buffer: 0; Tabpage: 2; Window: 1
        std::cout << type_name(v) << std::endl;
        std::cout << int(v.as_ext().type()) << std::endl;
        std::cout << v.as_ext().size() << std::endl;
        res = int(v.as_ext().data()[0]);
    }
    else std::cout << "invalid response type" << std::endl; //TODO: add error handler
}

template<typename...U>
void NvimRPC::call(const std::string &method, Object& res, const U& ...u) {
    Object v = do_call(method, u...);
    // std::cout << "Object NvimRPC::call" << std::endl;
    res = v;
}

template<typename...U>
void NvimRPC::call(const std::string &method, std::nullptr_t res, const U&...u) {
    do_call(method, u...);
    // std::cout << "void NvimRPC::call" << std::endl;
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

    // msgpack::object_handle oh = msgpack::unpack(sbuf.data(), sbuf.size());
    // msgpack::object deserialized = oh.get();
    // std::cout << "sbuf = " << deserialized << std::endl;

    socket_.write(sbuf.data(), sbuf.size(), 5);

    msgpack::unpacked result = socket_.read2(5);
    msgpack::object obj(result.get());

    //TODO: full-state response handler should be implemented

    // std::cout << "res = " << obj << std::endl;
    msgpack::type::tuple<int64_t, int64_t, Object, Object> dst;
    // [type=1, msgid(uint), error(str?), result(...)]
    obj.convert(dst);
    return dst.get<3>();
}

template<typename...U>
void NvimRPC::async_call(const std::string &method, const U&...u) {
    msgpack::sbuffer sbuf;
    detail::Packer pk(&sbuf);
    pk.pack_array(4) << (uint64_t)REQUEST
                     << msgid_++
                     << method;

    pk.pack_array(sizeof...(u));
    detail::pack(pk, u...);

    socket_.write(sbuf.data(), sbuf.size(), 5);
}

bool NvimRPC::send_response(uint64_t msgid, const Object &err, const Object &res)
{
    msgpack::sbuffer sbuf;
    detail::Packer pk(&sbuf);
    pk.pack_array(4) << (uint64_t)RESPONSE
                     << msgid
                     << err
                     << res;

    socket_.write(sbuf.data(), sbuf.size(), 5);

    return true;
}

} //namespace nvim
