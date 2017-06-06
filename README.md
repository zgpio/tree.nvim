# neovim.cpp

Boost::Asio based client library for talking with NeoVim process via it's msgpack-rpc API. (**in progress**)

Depends on: [msgpack-c](https://github.com/msgpack/msgpack-c), Jinja2, and Boost Libraries.

## Usage

```cpp
#include "nvim.hpp"

int main() {
    nvim::Nvim nvim;
    nvim.connect_tcp("localhost", "6666");
    nvim.nvim_eval("\( 3 + 2 \) \* 4");
    std::cout << "get_current_line = " << nvim.nvim_get_current_line() << std::endl;
    nvim.vim_set_current_line("testhogefuga");
    
    for(;;) {}

    return 0;
}
```
