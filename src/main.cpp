#include "nvim.hpp"
#include "app/app.h"
#include "util.h"
#include <iostream>
#include <string>
#include <locale>

using std::cerr;
using std::cout;
using std::endl;
using std::string;
#define BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT

// TODO: 临时
void eventloop(nvim::Nvim &nvim) {
    using nvim::Object;
    nvim::Array info = nvim.get_api_info();
    int chan_id = info[0].as_uint64_t();
    cout << "type(api-metadata): " << type_name(info[1]) << endl;
    cout << "Channel Id: " << chan_id << endl;

    tree::App app(&nvim, chan_id);

    string line = nvim.get_current_line();
    cout << "get_current_line = " << line << endl;

    cout << "eventloop started" << endl;
    while(true) {
        msgpack::unpacked result;
        try {
            result = nvim.client_.socket_.read2(10);
        }catch(std::exception& e) {
            cout << e.what() << endl;
            continue;
        }
        msgpack::object obj(result.get());
        if (obj.type != msgpack::type::ARRAY) {
            cerr << "Received Invalid msgpack: not an array";
            continue;
        }

        uint64_t type = obj.via.array.ptr[0].via.u64;

        cout << "res = " << obj << endl;
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
            {
                // [type(0), msgid, method, params]
                msgpack::type::tuple<int64_t, int64_t, Object, Object> msg;
                obj.convert(msg);
                int msgid = msg.get<1>();
                string method = msg.get<2>().as_string();
                cout << "msgid: " << msgid << endl;
                cout << "method: " << method << endl;
                cout << "type(msg.get<3>()): " << type_name(msg.get<3>()) << endl;
                auto argv = msg.get<3>().as_vector();

                // 涉及 tree.nvim 通信协议
                auto args = argv.front().as_vector();

                app.handleRequest(nvim.client_, msgid, method, args);
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
                string method = msg.get<1>().as_string();
                cout << "method: " << method << endl;
                cout << "type(msg.get<2>()): " << type_name(msg.get<2>()) << endl;
                auto argv = msg.get<2>().as_vector();

                // 涉及 tree.nvim 通信协议
                auto args = argv.front().as_vector();

                app.handleNvimNotification(method, args);
            }

            break;
        default:
            cerr << "Unsupported msg type" << type;
        }
    }
}

int main(int argc, char *argv[])
{
    cout << "argc:" << argc << " argv[1]:"<< argv[1] << endl;
    std::locale::global(std::locale(""));
    nvim::Nvim nvim;
    // nvim.connect_tcp("localhost", "6666");
    nvim.connect_pipe(argv[1]);

    string expr = "( 3 + 2 ) * 4";
    nvim::Object rv = nvim.eval(expr);
    cout << type_name(rv) << endl;
    uint64_t res = rv.as_uint64_t();
    printf("nvim_eval('%s') = %llu\n", expr.c_str(), res);

    eventloop(nvim);

    return 0;
}
