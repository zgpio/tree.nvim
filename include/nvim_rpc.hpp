#ifndef NEOVIM_CPP__NVIM_RPC_HPP_
#define NEOVIM_CPP__NVIM_RPC_HPP_
#define BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT

#include "msgpack.hpp"
#include <string>
#include <iostream>
#include <sstream>

#include "socket.hpp"

namespace nvim {

using Integer = int64_t;
using Window = Integer;
using Buffer = Integer;
using Tabpage = Integer;
using Object = msgpack::type::variant;
using Map = std::multimap<Object, Object>;
using Dictionary = Map;
using Array = std::vector<Object>;

class NvimRPC {

    enum {
        REQUEST  = 0,
        RESPONSE = 1,
        NOTIFY   = 2
    };

public:
    NvimRPC() :
        msgid_(0) {
    }

    void connect_tcp(const std::string &host,
            const std::string &service, double timeout_sec) {
        socket_.connect_tcp(host, service, timeout_sec);
    }

    template<typename T, typename...U>
    void call(const std::string &method, T& res, const U&...u);

    template<typename...U>
    void call(const std::string &method, Integer& res, const U&...u);

    template<typename...U>
    void call(const std::string &method, Object& res, const U&...u);

    template<typename...U>
    void call(const std::string &method, nullptr_t res, const U&...u);

    // TODO: 临时
    void eventloop() {
        using std::cerr;
        using std::string;
        std::cout << "eventloop started" << std::endl;
        while(true) {
            msgpack::unpacked result = socket_.read2(10);
            msgpack::object obj(result.get());
            if (obj.type != msgpack::type::ARRAY) {
                cerr << "Received Invalid msgpack: not an array";
                return;
            }

            uint64_t type = obj.via.array.ptr[0].via.u64;

            switch(type) {
            case 0:
                if (obj.via.array.ptr[1].type != msgpack::type::POSITIVE_INTEGER) {
                    cerr << "Received Invalid request: msg id MUST be a positive integer";
                    return;
                }
                if (obj.via.array.ptr[2].type != msgpack::type::BIN &&
                        obj.via.array.ptr[2].type != msgpack::type::STR) {
                    cerr << "Received Invalid request: method MUST be a String" << obj.via.array.ptr[2];
                    return;
                }
                if (obj.via.array.ptr[3].type != msgpack::type::ARRAY) {
                    cerr << "Invalid request: arguments MUST be an array";
                    return;
                }
                break;
            case 1:
                if (obj.via.array.ptr[1].type != msgpack::type::POSITIVE_INTEGER) {
                    cerr << "Received Invalid response: msg id MUST be a positive integer";
                    return;
                }
                break;
            case 2:
                {
                    Object methodName;
                    // [type(2), method, params]
                    msgpack::type::tuple<int64_t, Object, Object> msg;
                    obj.convert(msg);
                    std::cout << msg.get<1>().as_string() << std::endl;
                }

                break;
            default:
                cerr << "Unsupported msg type" << type;
            }
            std::cout << "res = " << obj << std::endl;
        }
    }
private:
    template<typename...U>
    Object do_call(const std::string &method, const U&...u);

    uint64_t msgid_;
    Socket socket_;

};

} //namespace nvim

#include "impl/nvim_rpc.hpp"

#endif //NEOVIM_CPP__NVIM_RPC_HPP_

