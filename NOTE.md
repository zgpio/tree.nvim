* https://github.com/tsubota-kouga/neovim.cpp 2019-3-17 -> 只有这一个fork有新的提交(2019-11-15记)
    * python2 -> python3:
        https://github.com/tsubota-kouga/neovim.cpp/commit/188fc95794da87ca54a66565efcd4d5b19aba038.patch
    * reset commit
    * remove boost::lambda

    ```sh
    git submodule update --init --recursive
    brew install boost
    brew info boost
    ```

* CMake
https://github.com/cquery-project/cquery/wiki/Compilation-database
    ```sh
    mkdir build
    (cd build; cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=YES ..)
    ln -s build/compile_commands.json
    ```

# 尝试实现异步接受rpc请求/通知
* [Boost异步套接字client](https://blog.csdn.net/AASE1235779/article/details/53957785)
* pynvim 的test中有广播的测试, broadcast needs `nvim_subscribe`
