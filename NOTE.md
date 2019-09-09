* https://github.com/tsubota-kouga/neovim.cpp 2019-3-17
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
