#include "neovim.hpp"

int main() {
    NeoVim client;

    client.send("vim_list_runtime_paths"); 
    
    sleep(1);
    client.send("vim_del_current_line");
    client.send("vim_list_runtime_paths"); 
    client.send("vim_set_current_line", "testhogefuga");

    for(;;) {}

    return 0;
}
