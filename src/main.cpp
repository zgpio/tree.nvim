#include "nvim.hpp"
#define BOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT 

int main() {
    nvim::Nvim nvim;
    nvim.connect_tcp("localhost", "6666");
    nvim.nvim_eval("\( 3 + 2 \) \* 4");
    std::cout << "get_current_line = " << nvim.nvim_get_current_line() << std::endl;
    nvim.vim_set_current_line("testhogefuga");
    
    for(;;) {}

    return 0;
}
