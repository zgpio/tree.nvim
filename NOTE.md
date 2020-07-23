* https://github.com/tsubota-kouga/neovim.cpp 2019-3-17 -> 只有这一个fork有新的提交(2019-11-15记)
    * python2 -> python3:
        https://github.com/tsubota-kouga/neovim.cpp/commit/188fc95794da87ca54a66565efcd4d5b19aba038.patch
    * reset commit
    * remove boost::lambda

# Build
```sh
# Ubuntu 16.04 上安装1.58版本的boost
sudo apt-get install libboost-all-dev

sudo add-apt-repository ppa:mhier/libboost-latest
sudo apt install libboost1.70-dev
# https://www.osetc.com/en/how-to-install-boost-on-ubuntu-16-04-18-04-linux.html
```
## CMake
- https://gitee.com/mirrors/boost
- [FindBoost](https://cmake.org/cmake/help/v3.15/module/FindBoost.html)
- CMAKE_EXPORT_COMPILE_COMMANDS
    ```sh
    # Ref to https://github.com/cquery-project/cquery/wiki/Compilation-database
    mkdir build
    (cd build; cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=YES ..)
    ln -s build/compile_commands.json
    ```
# Test
```vim
" config.vim
set rtp+=~/project/tree.nvim/runtime
so ~/project/tree.nvim/src/app/dev.vim
```
```sh
nvim -u config.vim --listen 127.0.0.1:6666
nvim -u config.vim --listen /tmp/xxxxxxxxx
```

# 尝试实现异步接受rpc请求/通知
* [Boost异步套接字client](https://www.boost.org/doc/libs/1_45_0/doc/html/boost_asio/example/timeouts/async_tcp_client.cpp)
* pynvim 的test中有广播的测试, broadcast needs `nvim_subscribe`

# 问题
* libc++abi.dylib: terminating with uncaught exception of type boost::filesystem::filesystem_error: boost::filesystem::directory_iterator::construct: Permission denied: "/etc/cups/certs"
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
* TODO: benchmark/performance 测试
* 设计用户接口:
    * 1) 需要变量参数的映射使用:map-<expr>, 其他不用, 因为对于常量调用没有必要每次通过tree#action转换命令,
         类似于tree#action('cd', getcwd())才需要每次求值;
    * 2)
* [msgpack controls a buffer](https://github.com/msgpack/msgpack-c/wiki/v2_0_cpp_unpacker)
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
    cout << "check_type:" << demangle(obj.type().name()) << endl;

    using boost::core::demangle;
    cout << demangle(rv.type().name()) << endl;
    cout << demangle(typeid(int).name()) << endl;

    // std::advance 和 std::next
    auto it = std::next(col_map[col].begin(), pos);
    ```
# NOTE
* [MessagePack for C++ 文档](http://c.msgpack.org/cpp/index.html)
* 返回值优化(RVO)
* Boost::Asio based client library for talking with NeoVim process via it's msgpack-rpc API.
  Depends on: [msgpack-c](https://github.com/msgpack/msgpack-c), Jinja2, and Boost Libraries.
* 对于Nvim类, 其method没有必要使用前缀`nvim_`;
  Nvim类应该隐藏NvimRPC的实现;
  NvimRPC类负责与nvim进程通信, 并对通信的细节进行封装.
* Integer res = client_.call("nvim_win_get_height", window);
* NvimRPC 的四个 call 函数
    ```cpp
    template<typename T, typename...U>  // (1)
    void call(const std::string &method, T& res, const U&...u);

    template<typename...U>  // (2)
    void call(const std::string &method, Integer& res, const U&...u);

    template<typename...U>  // (3)
    void call(const std::string &method, Object& res, const U&...u);

    template<typename...U>  // (4)
    void call(const std::string &method, std::nullptr_t res, const U&...u);

    ```
    * 其中 (2),(3),(4) 是 (1) 的特化模板, 因为Integer、Object是T的特化
    * 重载要求 返回值必须通过引用或指针表现在参数列表中，否则同名函数同参数列表发生歧义
    * 对于无返回值的rpc调用应使用(4)
    * 这四个call如此统一其实是为了使gen template更加简单
    * 如果删除nullptr_t, 导致(4)与(1),(2),(3)发生无法区分
    * 去掉(4)中的nullptr_t导致变参模板无法区分, 例如:
        `void NvimRPC::call(const std::string &method, Object& res, const U& ...u) ` 与
        `void NvimRPC::call(const std::string &method, nullptr_t res, const U&...u)`
* include/nvim_rpc.hpp 中的socket_临时的被设置为public
* read2临时命名，将读到的结果进行了解包
* run_one会运行事件处理循环，直到至少执行完成一个异步操作回调
* `cout << ec.message() << endl;` 显示错误码的详细信息, error::would_block => Resource temporarily unavailable
* 理解UTF-8与Unicode
    ```cpp
    // linux下wchar占4个字节
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
