#include "nvim.hpp"

int main() {
    nvim::Nvim nvim;
    nvim.nvim_eval("\( 3 + 2 \) \* 4");
    std::cout << "get_current_line = " << nvim.nvim_get_current_line() << std::endl;
    nvim.vim_set_current_line("testhogefuga");
    
    for(;;) {}

    return 0;
}
