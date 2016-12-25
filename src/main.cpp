#include "neovim.hpp"

int main() {
    NeoVim client;

    client.send("vim_list_runtime_paths"); 
    
    sleep(1);
    client.send("vim_del_current_line");
    client.send("vim_list_runtime_paths"); 

    for(;;) {}

    return 0;
}
