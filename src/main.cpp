#include "nvim.hpp"

int main() {
    Nvim nvim;
    nvim.nvim_eval("\( 3 + 2 \) \* 4");
    nvim.nvim_get_current_line();
    //nvim.vim_set_current_line("testhogefuga");
    
    for(;;) {}

    return 0;
}
