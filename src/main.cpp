#include "neovim.hpp"

int main() {
    NeoVim client;

    client.send("vim_del_current_line");
    client.send("vim_list_runtime_paths"); 
    client.send("vim_set_current_line", "testhogefuga");
    client.send("nvim_eval", "\( 3 + 2 \) \* 4");
    //client.send("ui_try_resize", 360, 240);
    //client.send("window_is_valid", 0);
    client.send("nvim_buf_set_var", 1, "a_buf", "test");

    for(;;) {}

    return 0;
}
