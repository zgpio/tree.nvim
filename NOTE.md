* https://github.com/tsubota-kouga/neovim.cpp 2019-3-17 -> 只有这一个fork有新的提交(2019-11-15记)
    * python2 -> python3:
        https://github.com/tsubota-kouga/neovim.cpp/commit/188fc95794da87ca54a66565efcd4d5b19aba038.patch
    * reset commit
    * remove boost::lambda

    ```sh
    git clone --recursive https://gitee.com/zgpio/nvim.cpp
    git submodule update --init --recursive
    brew install boost
    brew info boost

    cmake -S ~/Documents/nvim.cpp/ -B ~/Documents/nvim.cpp/build
    ```

* CMake
https://github.com/cquery-project/cquery/wiki/Compilation-database
    ```sh
    mkdir build
    (cd build; cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=YES ..)
    ln -s build/compile_commands.json
    ```
# Build
```sh
# Ubuntu 16.04 上安装1.58版本的boost
sudo apt-get install libboost-all-dev

sudo add-apt-repository ppa:mhier/libboost-latest
sudo apt install libboost1.70-dev
```

# 尝试实现异步接受rpc请求/通知
* [Boost异步套接字client](https://www.boost.org/doc/libs/1_45_0/doc/html/boost_asio/example/timeouts/async_tcp_client.cpp)
* pynvim 的test中有广播的测试, broadcast needs `nvim_subscribe`

# 问题
* initial_buffer_size=25*1024ul 时读取nvim_get_api_info()的结果时崩溃
    ```cpp
    msgpack::unpacker unpacker(
            [](msgpack::type::object_type /*type*/, std::size_t /*len*/, void*) -> bool { return true; },
            MSGPACK_NULLPTR,
            25*1024ul
            );
    // unpacker.reserve_buffer(32*1024ul);
    ```
* initial_buffer_size=25 时unpack得到的消息可能出现错误, 设置为17不会, 设置得这么小是为了验证reserve buffer正确工作.
* [msgpack controls a buffer](https://github.com/msgpack/msgpack-c/wiki/v2_0_cpp_unpacker)
* try
    ```cpp
    try {
    }catch(std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    ```
* 验证消息传递
    ```vim
    while 1
        call rpcnotify(g:cpp_chan_id, "helloworld\n",[1,2,[1,2],{1:2,2:3}])
        sleep 10m
    endw
    ```
* history
    ```cpp
    using boost::core::demangle;
    std::cout << "check_type:" << demangle(obj.type().name()) << std::endl;

    using boost::core::demangle;
    std::cout << demangle(rv.type().name()) << std::endl;
    std::cout << demangle(typeid(int).name()) << std::endl;

    // std::advance 和 std::next
    auto it = std::next(col_map[col].begin(), pos);
    ```
* NOTE
    * 理解UTF-8与Unicode
    ```cpp
    string s = "中国"; // (与源文件编码(UTF-8)一致?) byte array
    cout << s.at(0) << s.at(1) << s.at(2) << endl; // 中

    wstring ws(L"中国");
    cout << ws.size() << endl; // 2
    cout << "ws[0]: " << ws[0] << endl; // 中

    cout << wcwidth(L'中') << endl; // 2; 如果不设置locale, 输出-1.
    cout << "sizeof(wchar_t): "<< sizeof(wchar_t) << endl; // 4
    ```
    * boost::filesystem::path 支持 cout
    * [STL map, hash_map, unordered_map区别](https://blog.csdn.net/haluoluo211/article/details/80877558)
    * `__PRETTY_FUNCTION__` 不是标准预定义宏(Predefined Macros)
    * :h api-types
    * 去掉nullptr_t导致变参模板无法区分, 例如:
        `void NvimRPC::call(const std::string &method, Object& res, const U& ...u) ` 与
        `void NvimRPC::call(const std::string &method, nullptr_t res, const U&...u)`
