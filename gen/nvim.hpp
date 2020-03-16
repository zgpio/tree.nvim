// Auto generated

#ifndef NEOVIM_CPP__NVIM_HPP
#define NEOVIM_CPP__NVIM_HPP

#include "nvim_rpc.hpp"

namespace nvim {

class Nvim {
public:
    void connect_tcp(const std::string &host,
            const std::string &service, double timeout_sec = 1.0);
    void connect_pipe(const std::string &name,
            double timeout_sec = 1.0);


    Integer buf_line_count(Buffer buffer);
    bool buf_attach(Buffer buffer, bool send_buffer, const Dictionary& opts);
    bool buf_detach(Buffer buffer);
    std::vector<std::string> buf_get_lines(Buffer buffer, Integer start, Integer end, bool strict_indexing);
    void buf_set_lines(Buffer buffer, Integer start, Integer end, bool strict_indexing, const std::vector<std::string>& replacement);
    Integer buf_get_offset(Buffer buffer, Integer index);
    Object buf_get_var(Buffer buffer, const std::string& name);
    Integer buf_get_changedtick(Buffer buffer);
    std::vector<Dictionary> buf_get_keymap(Buffer buffer, const std::string& mode);
    void buf_set_keymap(Buffer buffer, const std::string& mode, const std::string& lhs, const std::string& rhs, const Dictionary& opts);
    void buf_del_keymap(Buffer buffer, const std::string& mode, const std::string& lhs);
    Dictionary buf_get_commands(Buffer buffer, const Dictionary& opts);
    void buf_set_var(Buffer buffer, const std::string& name, const Object& value);
    void buf_del_var(Buffer buffer, const std::string& name);
    Object buf_get_option(Buffer buffer, const std::string& name);
    void buf_set_option(Buffer buffer, const std::string& name, const Object& value);
    std::string buf_get_name(Buffer buffer);
    void buf_set_name(Buffer buffer, const std::string& name);
    bool buf_is_loaded(Buffer buffer);
    bool buf_is_valid(Buffer buffer);
    std::vector<Integer> buf_get_mark(Buffer buffer, const std::string& name);
    Integer buf_add_highlight(Buffer buffer, Integer ns_id, const std::string& hl_group, Integer line, Integer col_start, Integer col_end);
    void buf_clear_namespace(Buffer buffer, Integer ns_id, Integer line_start, Integer line_end);
    void buf_clear_highlight(Buffer buffer, Integer ns_id, Integer line_start, Integer line_end);
    Integer buf_set_virtual_text(Buffer buffer, Integer ns_id, Integer line, const Array& chunks, const Dictionary& opts);
    std::vector<Window> tabpage_list_wins(Tabpage tabpage);
    Object tabpage_get_var(Tabpage tabpage, const std::string& name);
    void tabpage_set_var(Tabpage tabpage, const std::string& name, const Object& value);
    void tabpage_del_var(Tabpage tabpage, const std::string& name);
    Window tabpage_get_win(Tabpage tabpage);
    Integer tabpage_get_number(Tabpage tabpage);
    bool tabpage_is_valid(Tabpage tabpage);
    void ui_attach(Integer width, Integer height, const Dictionary& options);
    void ui_detach();
    void ui_try_resize(Integer width, Integer height);
    void ui_set_option(const std::string& name, const Object& value);
    void ui_try_resize_grid(Integer grid, Integer width, Integer height);
    void ui_pum_set_height(Integer height);
    void command(const std::string& command);
    Dictionary get_hl_by_name(const std::string& name, bool rgb);
    Dictionary get_hl_by_id(Integer hl_id, bool rgb);
    void feedkeys(const std::string& keys, const std::string& mode, bool escape_csi);
    Integer input(const std::string& keys);
    void input_mouse(const std::string& button, const std::string& action, const std::string& modifier, Integer grid, Integer row, Integer col);
    std::string replace_termcodes(const std::string& str, bool from_part, bool do_lt, bool special);
    std::string command_output(const std::string& command);
    Object eval(const std::string& expr);
    Object execute_lua(const std::string& code, const Array& args);
    Object call_function(const std::string& fn, const Array& args);
    Object call_dict_function(const Object& dict, const std::string& fn, const Array& args);
    Integer strwidth(const std::string& text);
    std::vector<std::string> list_runtime_paths();
    void set_current_dir(const std::string& dir);
    std::string get_current_line();
    void set_current_line(const std::string& line);
    void del_current_line();
    Object get_var(const std::string& name);
    void set_var(const std::string& name, const Object& value);
    void del_var(const std::string& name);
    Object get_vvar(const std::string& name);
    void set_vvar(const std::string& name, const Object& value);
    Object get_option(const std::string& name);
    void set_option(const std::string& name, const Object& value);
    void out_write(const std::string& str);
    void err_write(const std::string& str);
    void err_writeln(const std::string& str);
    std::vector<Buffer> list_bufs();
    Buffer get_current_buf();
    void set_current_buf(Buffer buffer);
    std::vector<Window> list_wins();
    Window get_current_win();
    void set_current_win(Window window);
    Buffer create_buf(bool listed, bool scratch);
    Window open_win(Buffer buffer, bool enter, const Dictionary& config);
    std::vector<Tabpage> list_tabpages();
    Tabpage get_current_tabpage();
    void set_current_tabpage(Tabpage tabpage);
    Integer create_namespace(const std::string& name);
    Dictionary get_namespaces();
    bool paste(const std::string& data, bool crlf, Integer phase);
    void put(const std::vector<std::string>& lines, const std::string& type, bool after, bool follow);
    void subscribe(const std::string& event);
    void unsubscribe(const std::string& event);
    Integer get_color_by_name(const std::string& name);
    Dictionary get_color_map();
    Dictionary get_context(const Dictionary& opts);
    Object load_context(const Dictionary& dict);
    Dictionary get_mode();
    std::vector<Dictionary> get_keymap(const std::string& mode);
    void set_keymap(const std::string& mode, const std::string& lhs, const std::string& rhs, const Dictionary& opts);
    void del_keymap(const std::string& mode, const std::string& lhs);
    Dictionary get_commands(const Dictionary& opts);
    Array get_api_info();
    void set_client_info(const std::string& name, const Dictionary& version, const std::string& type, const Dictionary& methods, const Dictionary& attributes);
    Dictionary get_chan_info(Integer chan);
    Array list_chans();
    Array call_atomic(const Array& calls);
    Dictionary parse_expression(const std::string& expr, const std::string& flags, bool highlight);
    Array list_uis();
    Array get_proc_children(Integer pid);
    Object get_proc(Integer pid);
    void select_popupmenu_item(Integer item, bool insert, bool finish, const Dictionary& opts);
    Buffer win_get_buf(Window window);
    void win_set_buf(Window window, Buffer buffer);
    std::vector<Integer> win_get_cursor(Window window);
    void win_set_cursor(Window window, const std::vector<Integer>& pos);
    Integer win_get_height(Window window);
    void win_set_height(Window window, Integer height);
    Integer win_get_width(Window window);
    void win_set_width(Window window, Integer width);
    Object win_get_var(Window window, const std::string& name);
    void win_set_var(Window window, const std::string& name, const Object& value);
    void win_del_var(Window window, const std::string& name);
    Object win_get_option(Window window, const std::string& name);
    void win_set_option(Window window, const std::string& name, const Object& value);
    std::vector<Integer> win_get_position(Window window);
    Tabpage win_get_tabpage(Window window);
    Integer win_get_number(Window window);
    bool win_is_valid(Window window);
    void win_set_config(Window window, const Dictionary& config);
    Dictionary win_get_config(Window window);
    void win_close(Window window, bool force);
    void async_buf_line_count(Buffer buffer);
    void async_buf_attach(Buffer buffer, bool send_buffer, const Dictionary& opts);
    void async_buf_detach(Buffer buffer);
    void async_buf_get_lines(Buffer buffer, Integer start, Integer end, bool strict_indexing);
    void async_buf_set_lines(Buffer buffer, Integer start, Integer end, bool strict_indexing, const std::vector<std::string>& replacement);
    void async_buf_get_offset(Buffer buffer, Integer index);
    void async_buf_get_var(Buffer buffer, const std::string& name);
    void async_buf_get_changedtick(Buffer buffer);
    void async_buf_get_keymap(Buffer buffer, const std::string& mode);
    void async_buf_set_keymap(Buffer buffer, const std::string& mode, const std::string& lhs, const std::string& rhs, const Dictionary& opts);
    void async_buf_del_keymap(Buffer buffer, const std::string& mode, const std::string& lhs);
    void async_buf_get_commands(Buffer buffer, const Dictionary& opts);
    void async_buf_set_var(Buffer buffer, const std::string& name, const Object& value);
    void async_buf_del_var(Buffer buffer, const std::string& name);
    void async_buf_get_option(Buffer buffer, const std::string& name);
    void async_buf_set_option(Buffer buffer, const std::string& name, const Object& value);
    void async_buf_get_name(Buffer buffer);
    void async_buf_set_name(Buffer buffer, const std::string& name);
    void async_buf_is_loaded(Buffer buffer);
    void async_buf_is_valid(Buffer buffer);
    void async_buf_get_mark(Buffer buffer, const std::string& name);
    void async_buf_add_highlight(Buffer buffer, Integer ns_id, const std::string& hl_group, Integer line, Integer col_start, Integer col_end);
    void async_buf_clear_namespace(Buffer buffer, Integer ns_id, Integer line_start, Integer line_end);
    void async_buf_clear_highlight(Buffer buffer, Integer ns_id, Integer line_start, Integer line_end);
    void async_buf_set_virtual_text(Buffer buffer, Integer ns_id, Integer line, const Array& chunks, const Dictionary& opts);
    void async_tabpage_list_wins(Tabpage tabpage);
    void async_tabpage_get_var(Tabpage tabpage, const std::string& name);
    void async_tabpage_set_var(Tabpage tabpage, const std::string& name, const Object& value);
    void async_tabpage_del_var(Tabpage tabpage, const std::string& name);
    void async_tabpage_get_win(Tabpage tabpage);
    void async_tabpage_get_number(Tabpage tabpage);
    void async_tabpage_is_valid(Tabpage tabpage);
    void async_ui_attach(Integer width, Integer height, const Dictionary& options);
    void async_ui_detach();
    void async_ui_try_resize(Integer width, Integer height);
    void async_ui_set_option(const std::string& name, const Object& value);
    void async_ui_try_resize_grid(Integer grid, Integer width, Integer height);
    void async_ui_pum_set_height(Integer height);
    void async_command(const std::string& command);
    void async_get_hl_by_name(const std::string& name, bool rgb);
    void async_get_hl_by_id(Integer hl_id, bool rgb);
    void async_feedkeys(const std::string& keys, const std::string& mode, bool escape_csi);
    void async_input(const std::string& keys);
    void async_input_mouse(const std::string& button, const std::string& action, const std::string& modifier, Integer grid, Integer row, Integer col);
    void async_replace_termcodes(const std::string& str, bool from_part, bool do_lt, bool special);
    void async_command_output(const std::string& command);
    void async_eval(const std::string& expr);
    void async_execute_lua(const std::string& code, const Array& args);
    void async_call_function(const std::string& fn, const Array& args);
    void async_call_dict_function(const Object& dict, const std::string& fn, const Array& args);
    void async_strwidth(const std::string& text);
    void async_list_runtime_paths();
    void async_set_current_dir(const std::string& dir);
    void async_get_current_line();
    void async_set_current_line(const std::string& line);
    void async_del_current_line();
    void async_get_var(const std::string& name);
    void async_set_var(const std::string& name, const Object& value);
    void async_del_var(const std::string& name);
    void async_get_vvar(const std::string& name);
    void async_set_vvar(const std::string& name, const Object& value);
    void async_get_option(const std::string& name);
    void async_set_option(const std::string& name, const Object& value);
    void async_out_write(const std::string& str);
    void async_err_write(const std::string& str);
    void async_err_writeln(const std::string& str);
    void async_list_bufs();
    void async_get_current_buf();
    void async_set_current_buf(Buffer buffer);
    void async_list_wins();
    void async_get_current_win();
    void async_set_current_win(Window window);
    void async_create_buf(bool listed, bool scratch);
    void async_open_win(Buffer buffer, bool enter, const Dictionary& config);
    void async_list_tabpages();
    void async_get_current_tabpage();
    void async_set_current_tabpage(Tabpage tabpage);
    void async_create_namespace(const std::string& name);
    void async_get_namespaces();
    void async_paste(const std::string& data, bool crlf, Integer phase);
    void async_put(const std::vector<std::string>& lines, const std::string& type, bool after, bool follow);
    void async_subscribe(const std::string& event);
    void async_unsubscribe(const std::string& event);
    void async_get_color_by_name(const std::string& name);
    void async_get_color_map();
    void async_get_context(const Dictionary& opts);
    void async_load_context(const Dictionary& dict);
    void async_get_mode();
    void async_get_keymap(const std::string& mode);
    void async_set_keymap(const std::string& mode, const std::string& lhs, const std::string& rhs, const Dictionary& opts);
    void async_del_keymap(const std::string& mode, const std::string& lhs);
    void async_get_commands(const Dictionary& opts);
    void async_get_api_info();
    void async_set_client_info(const std::string& name, const Dictionary& version, const std::string& type, const Dictionary& methods, const Dictionary& attributes);
    void async_get_chan_info(Integer chan);
    void async_list_chans();
    void async_call_atomic(const Array& calls);
    void async_parse_expression(const std::string& expr, const std::string& flags, bool highlight);
    void async_list_uis();
    void async_get_proc_children(Integer pid);
    void async_get_proc(Integer pid);
    void async_select_popupmenu_item(Integer item, bool insert, bool finish, const Dictionary& opts);
    void async_win_get_buf(Window window);
    void async_win_set_buf(Window window, Buffer buffer);
    void async_win_get_cursor(Window window);
    void async_win_set_cursor(Window window, const std::vector<Integer>& pos);
    void async_win_get_height(Window window);
    void async_win_set_height(Window window, Integer height);
    void async_win_get_width(Window window);
    void async_win_set_width(Window window, Integer width);
    void async_win_get_var(Window window, const std::string& name);
    void async_win_set_var(Window window, const std::string& name, const Object& value);
    void async_win_del_var(Window window, const std::string& name);
    void async_win_get_option(Window window, const std::string& name);
    void async_win_set_option(Window window, const std::string& name, const Object& value);
    void async_win_get_position(Window window);
    void async_win_get_tabpage(Window window);
    void async_win_get_number(Window window);
    void async_win_is_valid(Window window);
    void async_win_set_config(Window window, const Dictionary& config);
    void async_win_get_config(Window window);
    void async_win_close(Window window, bool force);

public:
    NvimRPC client_;

};

} //namespace nvim

#endif //NEOVIM_CPP__NVIM_HPP
