// Auto generated
#include "nvim_rpc.hpp"
#include "nvim.hpp"

namespace nvim {

void Nvim::connect_tcp(const std::string &host,
        const std::string &service, double timeout_sec) {
    client_.connect_tcp(host, service, timeout_sec);
}

void Nvim::connect_pipe(const std::string &name,
        double timeout_sec) {
    client_.connect_pipe(name, timeout_sec);
}


Integer Nvim::buf_line_count(Buffer buffer) {
    Integer res;
    client_.call("nvim_buf_line_count", res, buffer);
    return res;
}

bool Nvim::buf_attach(Buffer buffer, bool send_buffer, const Dictionary& opts) {
    bool res;
    client_.call("nvim_buf_attach", res, buffer, send_buffer, opts);
    return res;
}

bool Nvim::buf_detach(Buffer buffer) {
    bool res;
    client_.call("nvim_buf_detach", res, buffer);
    return res;
}

std::vector<std::string> Nvim::buf_get_lines(Buffer buffer, Integer start, Integer end, bool strict_indexing) {
    std::vector<std::string> res;
    client_.call("nvim_buf_get_lines", res, buffer, start, end, strict_indexing);
    return res;
}

void Nvim::buf_set_lines(Buffer buffer, Integer start, Integer end, bool strict_indexing, const std::vector<std::string>& replacement) {
    client_.call("nvim_buf_set_lines", nullptr, buffer, start, end, strict_indexing, replacement);
}

Integer Nvim::buf_get_offset(Buffer buffer, Integer index) {
    Integer res;
    client_.call("nvim_buf_get_offset", res, buffer, index);
    return res;
}

Object Nvim::buf_get_var(Buffer buffer, const std::string& name) {
    Object res;
    client_.call("nvim_buf_get_var", res, buffer, name);
    return res;
}

Integer Nvim::buf_get_changedtick(Buffer buffer) {
    Integer res;
    client_.call("nvim_buf_get_changedtick", res, buffer);
    return res;
}

std::vector<Dictionary> Nvim::buf_get_keymap(Buffer buffer, const std::string& mode) {
    std::vector<Dictionary> res;
    client_.call("nvim_buf_get_keymap", res, buffer, mode);
    return res;
}

void Nvim::buf_set_keymap(Buffer buffer, const std::string& mode, const std::string& lhs, const std::string& rhs, const Dictionary& opts) {
    client_.call("nvim_buf_set_keymap", nullptr, buffer, mode, lhs, rhs, opts);
}

void Nvim::buf_del_keymap(Buffer buffer, const std::string& mode, const std::string& lhs) {
    client_.call("nvim_buf_del_keymap", nullptr, buffer, mode, lhs);
}

Dictionary Nvim::buf_get_commands(Buffer buffer, const Dictionary& opts) {
    Dictionary res;
    client_.call("nvim_buf_get_commands", res, buffer, opts);
    return res;
}

void Nvim::buf_set_var(Buffer buffer, const std::string& name, const Object& value) {
    client_.call("nvim_buf_set_var", nullptr, buffer, name, value);
}

void Nvim::buf_del_var(Buffer buffer, const std::string& name) {
    client_.call("nvim_buf_del_var", nullptr, buffer, name);
}

Object Nvim::buf_get_option(Buffer buffer, const std::string& name) {
    Object res;
    client_.call("nvim_buf_get_option", res, buffer, name);
    return res;
}

void Nvim::buf_set_option(Buffer buffer, const std::string& name, const Object& value) {
    client_.call("nvim_buf_set_option", nullptr, buffer, name, value);
}

std::string Nvim::buf_get_name(Buffer buffer) {
    std::string res;
    client_.call("nvim_buf_get_name", res, buffer);
    return res;
}

void Nvim::buf_set_name(Buffer buffer, const std::string& name) {
    client_.call("nvim_buf_set_name", nullptr, buffer, name);
}

bool Nvim::buf_is_loaded(Buffer buffer) {
    bool res;
    client_.call("nvim_buf_is_loaded", res, buffer);
    return res;
}

bool Nvim::buf_is_valid(Buffer buffer) {
    bool res;
    client_.call("nvim_buf_is_valid", res, buffer);
    return res;
}

std::vector<Integer> Nvim::buf_get_mark(Buffer buffer, const std::string& name) {
    std::vector<Integer> res;
    client_.call("nvim_buf_get_mark", res, buffer, name);
    return res;
}

Integer Nvim::buf_add_highlight(Buffer buffer, Integer ns_id, const std::string& hl_group, Integer line, Integer col_start, Integer col_end) {
    Integer res;
    client_.call("nvim_buf_add_highlight", res, buffer, ns_id, hl_group, line, col_start, col_end);
    return res;
}

void Nvim::buf_clear_namespace(Buffer buffer, Integer ns_id, Integer line_start, Integer line_end) {
    client_.call("nvim_buf_clear_namespace", nullptr, buffer, ns_id, line_start, line_end);
}

void Nvim::buf_clear_highlight(Buffer buffer, Integer ns_id, Integer line_start, Integer line_end) {
    client_.call("nvim_buf_clear_highlight", nullptr, buffer, ns_id, line_start, line_end);
}

Integer Nvim::buf_set_virtual_text(Buffer buffer, Integer ns_id, Integer line, const Array& chunks, const Dictionary& opts) {
    Integer res;
    client_.call("nvim_buf_set_virtual_text", res, buffer, ns_id, line, chunks, opts);
    return res;
}

std::vector<Window> Nvim::tabpage_list_wins(Tabpage tabpage) {
    std::vector<Window> res;
    client_.call("nvim_tabpage_list_wins", res, tabpage);
    return res;
}

Object Nvim::tabpage_get_var(Tabpage tabpage, const std::string& name) {
    Object res;
    client_.call("nvim_tabpage_get_var", res, tabpage, name);
    return res;
}

void Nvim::tabpage_set_var(Tabpage tabpage, const std::string& name, const Object& value) {
    client_.call("nvim_tabpage_set_var", nullptr, tabpage, name, value);
}

void Nvim::tabpage_del_var(Tabpage tabpage, const std::string& name) {
    client_.call("nvim_tabpage_del_var", nullptr, tabpage, name);
}

Window Nvim::tabpage_get_win(Tabpage tabpage) {
    Window res;
    client_.call("nvim_tabpage_get_win", res, tabpage);
    return res;
}

Integer Nvim::tabpage_get_number(Tabpage tabpage) {
    Integer res;
    client_.call("nvim_tabpage_get_number", res, tabpage);
    return res;
}

bool Nvim::tabpage_is_valid(Tabpage tabpage) {
    bool res;
    client_.call("nvim_tabpage_is_valid", res, tabpage);
    return res;
}

void Nvim::ui_attach(Integer width, Integer height, const Dictionary& options) {
    client_.call("nvim_ui_attach", nullptr, width, height, options);
}

void Nvim::ui_detach() {
    client_.call("nvim_ui_detach", nullptr);
}

void Nvim::ui_try_resize(Integer width, Integer height) {
    client_.call("nvim_ui_try_resize", nullptr, width, height);
}

void Nvim::ui_set_option(const std::string& name, const Object& value) {
    client_.call("nvim_ui_set_option", nullptr, name, value);
}

void Nvim::ui_try_resize_grid(Integer grid, Integer width, Integer height) {
    client_.call("nvim_ui_try_resize_grid", nullptr, grid, width, height);
}

void Nvim::ui_pum_set_height(Integer height) {
    client_.call("nvim_ui_pum_set_height", nullptr, height);
}

void Nvim::command(const std::string& command) {
    client_.call("nvim_command", nullptr, command);
}

Dictionary Nvim::get_hl_by_name(const std::string& name, bool rgb) {
    Dictionary res;
    client_.call("nvim_get_hl_by_name", res, name, rgb);
    return res;
}

Dictionary Nvim::get_hl_by_id(Integer hl_id, bool rgb) {
    Dictionary res;
    client_.call("nvim_get_hl_by_id", res, hl_id, rgb);
    return res;
}

void Nvim::feedkeys(const std::string& keys, const std::string& mode, bool escape_csi) {
    client_.call("nvim_feedkeys", nullptr, keys, mode, escape_csi);
}

Integer Nvim::input(const std::string& keys) {
    Integer res;
    client_.call("nvim_input", res, keys);
    return res;
}

void Nvim::input_mouse(const std::string& button, const std::string& action, const std::string& modifier, Integer grid, Integer row, Integer col) {
    client_.call("nvim_input_mouse", nullptr, button, action, modifier, grid, row, col);
}

std::string Nvim::replace_termcodes(const std::string& str, bool from_part, bool do_lt, bool special) {
    std::string res;
    client_.call("nvim_replace_termcodes", res, str, from_part, do_lt, special);
    return res;
}

std::string Nvim::command_output(const std::string& command) {
    std::string res;
    client_.call("nvim_command_output", res, command);
    return res;
}

Object Nvim::eval(const std::string& expr) {
    Object res;
    client_.call("nvim_eval", res, expr);
    return res;
}

Object Nvim::execute_lua(const std::string& code, const Array& args) {
    Object res;
    client_.call("nvim_execute_lua", res, code, args);
    return res;
}

Object Nvim::call_function(const std::string& fn, const Array& args) {
    Object res;
    client_.call("nvim_call_function", res, fn, args);
    return res;
}

Object Nvim::call_dict_function(const Object& dict, const std::string& fn, const Array& args) {
    Object res;
    client_.call("nvim_call_dict_function", res, dict, fn, args);
    return res;
}

Integer Nvim::strwidth(const std::string& text) {
    Integer res;
    client_.call("nvim_strwidth", res, text);
    return res;
}

std::vector<std::string> Nvim::list_runtime_paths() {
    std::vector<std::string> res;
    client_.call("nvim_list_runtime_paths", res);
    return res;
}

void Nvim::set_current_dir(const std::string& dir) {
    client_.call("nvim_set_current_dir", nullptr, dir);
}

std::string Nvim::get_current_line() {
    std::string res;
    client_.call("nvim_get_current_line", res);
    return res;
}

void Nvim::set_current_line(const std::string& line) {
    client_.call("nvim_set_current_line", nullptr, line);
}

void Nvim::del_current_line() {
    client_.call("nvim_del_current_line", nullptr);
}

Object Nvim::get_var(const std::string& name) {
    Object res;
    client_.call("nvim_get_var", res, name);
    return res;
}

void Nvim::set_var(const std::string& name, const Object& value) {
    client_.call("nvim_set_var", nullptr, name, value);
}

void Nvim::del_var(const std::string& name) {
    client_.call("nvim_del_var", nullptr, name);
}

Object Nvim::get_vvar(const std::string& name) {
    Object res;
    client_.call("nvim_get_vvar", res, name);
    return res;
}

void Nvim::set_vvar(const std::string& name, const Object& value) {
    client_.call("nvim_set_vvar", nullptr, name, value);
}

Object Nvim::get_option(const std::string& name) {
    Object res;
    client_.call("nvim_get_option", res, name);
    return res;
}

void Nvim::set_option(const std::string& name, const Object& value) {
    client_.call("nvim_set_option", nullptr, name, value);
}

void Nvim::out_write(const std::string& str) {
    client_.call("nvim_out_write", nullptr, str);
}

void Nvim::err_write(const std::string& str) {
    client_.call("nvim_err_write", nullptr, str);
}

void Nvim::err_writeln(const std::string& str) {
    client_.call("nvim_err_writeln", nullptr, str);
}

std::vector<Buffer> Nvim::list_bufs() {
    std::vector<Buffer> res;
    client_.call("nvim_list_bufs", res);
    return res;
}

Buffer Nvim::get_current_buf() {
    Buffer res;
    client_.call("nvim_get_current_buf", res);
    return res;
}

void Nvim::set_current_buf(Buffer buffer) {
    client_.call("nvim_set_current_buf", nullptr, buffer);
}

std::vector<Window> Nvim::list_wins() {
    std::vector<Window> res;
    client_.call("nvim_list_wins", res);
    return res;
}

Window Nvim::get_current_win() {
    Window res;
    client_.call("nvim_get_current_win", res);
    return res;
}

void Nvim::set_current_win(Window window) {
    client_.call("nvim_set_current_win", nullptr, window);
}

Buffer Nvim::create_buf(bool listed, bool scratch) {
    Buffer res;
    client_.call("nvim_create_buf", res, listed, scratch);
    return res;
}

Window Nvim::open_win(Buffer buffer, bool enter, const Dictionary& config) {
    Window res;
    client_.call("nvim_open_win", res, buffer, enter, config);
    return res;
}

std::vector<Tabpage> Nvim::list_tabpages() {
    std::vector<Tabpage> res;
    client_.call("nvim_list_tabpages", res);
    return res;
}

Tabpage Nvim::get_current_tabpage() {
    Tabpage res;
    client_.call("nvim_get_current_tabpage", res);
    return res;
}

void Nvim::set_current_tabpage(Tabpage tabpage) {
    client_.call("nvim_set_current_tabpage", nullptr, tabpage);
}

Integer Nvim::create_namespace(const std::string& name) {
    Integer res;
    client_.call("nvim_create_namespace", res, name);
    return res;
}

Dictionary Nvim::get_namespaces() {
    Dictionary res;
    client_.call("nvim_get_namespaces", res);
    return res;
}

bool Nvim::paste(const std::string& data, bool crlf, Integer phase) {
    bool res;
    client_.call("nvim_paste", res, data, crlf, phase);
    return res;
}

void Nvim::put(const std::vector<std::string>& lines, const std::string& type, bool after, bool follow) {
    client_.call("nvim_put", nullptr, lines, type, after, follow);
}

void Nvim::subscribe(const std::string& event) {
    client_.call("nvim_subscribe", nullptr, event);
}

void Nvim::unsubscribe(const std::string& event) {
    client_.call("nvim_unsubscribe", nullptr, event);
}

Integer Nvim::get_color_by_name(const std::string& name) {
    Integer res;
    client_.call("nvim_get_color_by_name", res, name);
    return res;
}

Dictionary Nvim::get_color_map() {
    Dictionary res;
    client_.call("nvim_get_color_map", res);
    return res;
}

Dictionary Nvim::get_context(const Dictionary& opts) {
    Dictionary res;
    client_.call("nvim_get_context", res, opts);
    return res;
}

Object Nvim::load_context(const Dictionary& dict) {
    Object res;
    client_.call("nvim_load_context", res, dict);
    return res;
}

Dictionary Nvim::get_mode() {
    Dictionary res;
    client_.call("nvim_get_mode", res);
    return res;
}

std::vector<Dictionary> Nvim::get_keymap(const std::string& mode) {
    std::vector<Dictionary> res;
    client_.call("nvim_get_keymap", res, mode);
    return res;
}

void Nvim::set_keymap(const std::string& mode, const std::string& lhs, const std::string& rhs, const Dictionary& opts) {
    client_.call("nvim_set_keymap", nullptr, mode, lhs, rhs, opts);
}

void Nvim::del_keymap(const std::string& mode, const std::string& lhs) {
    client_.call("nvim_del_keymap", nullptr, mode, lhs);
}

Dictionary Nvim::get_commands(const Dictionary& opts) {
    Dictionary res;
    client_.call("nvim_get_commands", res, opts);
    return res;
}

Array Nvim::get_api_info() {
    Array res;
    client_.call("nvim_get_api_info", res);
    return res;
}

void Nvim::set_client_info(const std::string& name, const Dictionary& version, const std::string& type, const Dictionary& methods, const Dictionary& attributes) {
    client_.call("nvim_set_client_info", nullptr, name, version, type, methods, attributes);
}

Dictionary Nvim::get_chan_info(Integer chan) {
    Dictionary res;
    client_.call("nvim_get_chan_info", res, chan);
    return res;
}

Array Nvim::list_chans() {
    Array res;
    client_.call("nvim_list_chans", res);
    return res;
}

Array Nvim::call_atomic(const Array& calls) {
    Array res;
    client_.call("nvim_call_atomic", res, calls);
    return res;
}

Dictionary Nvim::parse_expression(const std::string& expr, const std::string& flags, bool highlight) {
    Dictionary res;
    client_.call("nvim_parse_expression", res, expr, flags, highlight);
    return res;
}

Array Nvim::list_uis() {
    Array res;
    client_.call("nvim_list_uis", res);
    return res;
}

Array Nvim::get_proc_children(Integer pid) {
    Array res;
    client_.call("nvim_get_proc_children", res, pid);
    return res;
}

Object Nvim::get_proc(Integer pid) {
    Object res;
    client_.call("nvim_get_proc", res, pid);
    return res;
}

void Nvim::select_popupmenu_item(Integer item, bool insert, bool finish, const Dictionary& opts) {
    client_.call("nvim_select_popupmenu_item", nullptr, item, insert, finish, opts);
}

Buffer Nvim::win_get_buf(Window window) {
    Buffer res;
    client_.call("nvim_win_get_buf", res, window);
    return res;
}

void Nvim::win_set_buf(Window window, Buffer buffer) {
    client_.call("nvim_win_set_buf", nullptr, window, buffer);
}

std::vector<Integer> Nvim::win_get_cursor(Window window) {
    std::vector<Integer> res;
    client_.call("nvim_win_get_cursor", res, window);
    return res;
}

void Nvim::win_set_cursor(Window window, const std::vector<Integer>& pos) {
    client_.call("nvim_win_set_cursor", nullptr, window, pos);
}

Integer Nvim::win_get_height(Window window) {
    Integer res;
    client_.call("nvim_win_get_height", res, window);
    return res;
}

void Nvim::win_set_height(Window window, Integer height) {
    client_.call("nvim_win_set_height", nullptr, window, height);
}

Integer Nvim::win_get_width(Window window) {
    Integer res;
    client_.call("nvim_win_get_width", res, window);
    return res;
}

void Nvim::win_set_width(Window window, Integer width) {
    client_.call("nvim_win_set_width", nullptr, window, width);
}

Object Nvim::win_get_var(Window window, const std::string& name) {
    Object res;
    client_.call("nvim_win_get_var", res, window, name);
    return res;
}

void Nvim::win_set_var(Window window, const std::string& name, const Object& value) {
    client_.call("nvim_win_set_var", nullptr, window, name, value);
}

void Nvim::win_del_var(Window window, const std::string& name) {
    client_.call("nvim_win_del_var", nullptr, window, name);
}

Object Nvim::win_get_option(Window window, const std::string& name) {
    Object res;
    client_.call("nvim_win_get_option", res, window, name);
    return res;
}

void Nvim::win_set_option(Window window, const std::string& name, const Object& value) {
    client_.call("nvim_win_set_option", nullptr, window, name, value);
}

std::vector<Integer> Nvim::win_get_position(Window window) {
    std::vector<Integer> res;
    client_.call("nvim_win_get_position", res, window);
    return res;
}

Tabpage Nvim::win_get_tabpage(Window window) {
    Tabpage res;
    client_.call("nvim_win_get_tabpage", res, window);
    return res;
}

Integer Nvim::win_get_number(Window window) {
    Integer res;
    client_.call("nvim_win_get_number", res, window);
    return res;
}

bool Nvim::win_is_valid(Window window) {
    bool res;
    client_.call("nvim_win_is_valid", res, window);
    return res;
}

void Nvim::win_set_config(Window window, const Dictionary& config) {
    client_.call("nvim_win_set_config", nullptr, window, config);
}

Dictionary Nvim::win_get_config(Window window) {
    Dictionary res;
    client_.call("nvim_win_get_config", res, window);
    return res;
}

void Nvim::win_close(Window window, bool force) {
    client_.call("nvim_win_close", nullptr, window, force);
}



void Nvim::async_buf_line_count(Buffer buffer) {
    client_.async_call("nvim_buf_line_count", buffer);
}

void Nvim::async_buf_attach(Buffer buffer, bool send_buffer, const Dictionary& opts) {
    client_.async_call("nvim_buf_attach", buffer, send_buffer, opts);
}

void Nvim::async_buf_detach(Buffer buffer) {
    client_.async_call("nvim_buf_detach", buffer);
}

void Nvim::async_buf_get_lines(Buffer buffer, Integer start, Integer end, bool strict_indexing) {
    client_.async_call("nvim_buf_get_lines", buffer, start, end, strict_indexing);
}

void Nvim::async_buf_set_lines(Buffer buffer, Integer start, Integer end, bool strict_indexing, const std::vector<std::string>& replacement) {
    client_.async_call("nvim_buf_set_lines", buffer, start, end, strict_indexing, replacement);
}

void Nvim::async_buf_get_offset(Buffer buffer, Integer index) {
    client_.async_call("nvim_buf_get_offset", buffer, index);
}

void Nvim::async_buf_get_var(Buffer buffer, const std::string& name) {
    client_.async_call("nvim_buf_get_var", buffer, name);
}

void Nvim::async_buf_get_changedtick(Buffer buffer) {
    client_.async_call("nvim_buf_get_changedtick", buffer);
}

void Nvim::async_buf_get_keymap(Buffer buffer, const std::string& mode) {
    client_.async_call("nvim_buf_get_keymap", buffer, mode);
}

void Nvim::async_buf_set_keymap(Buffer buffer, const std::string& mode, const std::string& lhs, const std::string& rhs, const Dictionary& opts) {
    client_.async_call("nvim_buf_set_keymap", buffer, mode, lhs, rhs, opts);
}

void Nvim::async_buf_del_keymap(Buffer buffer, const std::string& mode, const std::string& lhs) {
    client_.async_call("nvim_buf_del_keymap", buffer, mode, lhs);
}

void Nvim::async_buf_get_commands(Buffer buffer, const Dictionary& opts) {
    client_.async_call("nvim_buf_get_commands", buffer, opts);
}

void Nvim::async_buf_set_var(Buffer buffer, const std::string& name, const Object& value) {
    client_.async_call("nvim_buf_set_var", buffer, name, value);
}

void Nvim::async_buf_del_var(Buffer buffer, const std::string& name) {
    client_.async_call("nvim_buf_del_var", buffer, name);
}

void Nvim::async_buf_get_option(Buffer buffer, const std::string& name) {
    client_.async_call("nvim_buf_get_option", buffer, name);
}

void Nvim::async_buf_set_option(Buffer buffer, const std::string& name, const Object& value) {
    client_.async_call("nvim_buf_set_option", buffer, name, value);
}

void Nvim::async_buf_get_name(Buffer buffer) {
    client_.async_call("nvim_buf_get_name", buffer);
}

void Nvim::async_buf_set_name(Buffer buffer, const std::string& name) {
    client_.async_call("nvim_buf_set_name", buffer, name);
}

void Nvim::async_buf_is_loaded(Buffer buffer) {
    client_.async_call("nvim_buf_is_loaded", buffer);
}

void Nvim::async_buf_is_valid(Buffer buffer) {
    client_.async_call("nvim_buf_is_valid", buffer);
}

void Nvim::async_buf_get_mark(Buffer buffer, const std::string& name) {
    client_.async_call("nvim_buf_get_mark", buffer, name);
}

void Nvim::async_buf_add_highlight(Buffer buffer, Integer ns_id, const std::string& hl_group, Integer line, Integer col_start, Integer col_end) {
    client_.async_call("nvim_buf_add_highlight", buffer, ns_id, hl_group, line, col_start, col_end);
}

void Nvim::async_buf_clear_namespace(Buffer buffer, Integer ns_id, Integer line_start, Integer line_end) {
    client_.async_call("nvim_buf_clear_namespace", buffer, ns_id, line_start, line_end);
}

void Nvim::async_buf_clear_highlight(Buffer buffer, Integer ns_id, Integer line_start, Integer line_end) {
    client_.async_call("nvim_buf_clear_highlight", buffer, ns_id, line_start, line_end);
}

void Nvim::async_buf_set_virtual_text(Buffer buffer, Integer ns_id, Integer line, const Array& chunks, const Dictionary& opts) {
    client_.async_call("nvim_buf_set_virtual_text", buffer, ns_id, line, chunks, opts);
}

void Nvim::async_tabpage_list_wins(Tabpage tabpage) {
    client_.async_call("nvim_tabpage_list_wins", tabpage);
}

void Nvim::async_tabpage_get_var(Tabpage tabpage, const std::string& name) {
    client_.async_call("nvim_tabpage_get_var", tabpage, name);
}

void Nvim::async_tabpage_set_var(Tabpage tabpage, const std::string& name, const Object& value) {
    client_.async_call("nvim_tabpage_set_var", tabpage, name, value);
}

void Nvim::async_tabpage_del_var(Tabpage tabpage, const std::string& name) {
    client_.async_call("nvim_tabpage_del_var", tabpage, name);
}

void Nvim::async_tabpage_get_win(Tabpage tabpage) {
    client_.async_call("nvim_tabpage_get_win", tabpage);
}

void Nvim::async_tabpage_get_number(Tabpage tabpage) {
    client_.async_call("nvim_tabpage_get_number", tabpage);
}

void Nvim::async_tabpage_is_valid(Tabpage tabpage) {
    client_.async_call("nvim_tabpage_is_valid", tabpage);
}

void Nvim::async_ui_attach(Integer width, Integer height, const Dictionary& options) {
    client_.async_call("nvim_ui_attach", width, height, options);
}

void Nvim::async_ui_detach() {
    client_.async_call("nvim_ui_detach");
}

void Nvim::async_ui_try_resize(Integer width, Integer height) {
    client_.async_call("nvim_ui_try_resize", width, height);
}

void Nvim::async_ui_set_option(const std::string& name, const Object& value) {
    client_.async_call("nvim_ui_set_option", name, value);
}

void Nvim::async_ui_try_resize_grid(Integer grid, Integer width, Integer height) {
    client_.async_call("nvim_ui_try_resize_grid", grid, width, height);
}

void Nvim::async_ui_pum_set_height(Integer height) {
    client_.async_call("nvim_ui_pum_set_height", height);
}

void Nvim::async_command(const std::string& command) {
    client_.async_call("nvim_command", command);
}

void Nvim::async_get_hl_by_name(const std::string& name, bool rgb) {
    client_.async_call("nvim_get_hl_by_name", name, rgb);
}

void Nvim::async_get_hl_by_id(Integer hl_id, bool rgb) {
    client_.async_call("nvim_get_hl_by_id", hl_id, rgb);
}

void Nvim::async_feedkeys(const std::string& keys, const std::string& mode, bool escape_csi) {
    client_.async_call("nvim_feedkeys", keys, mode, escape_csi);
}

void Nvim::async_input(const std::string& keys) {
    client_.async_call("nvim_input", keys);
}

void Nvim::async_input_mouse(const std::string& button, const std::string& action, const std::string& modifier, Integer grid, Integer row, Integer col) {
    client_.async_call("nvim_input_mouse", button, action, modifier, grid, row, col);
}

void Nvim::async_replace_termcodes(const std::string& str, bool from_part, bool do_lt, bool special) {
    client_.async_call("nvim_replace_termcodes", str, from_part, do_lt, special);
}

void Nvim::async_command_output(const std::string& command) {
    client_.async_call("nvim_command_output", command);
}

void Nvim::async_eval(const std::string& expr) {
    client_.async_call("nvim_eval", expr);
}

void Nvim::async_execute_lua(const std::string& code, const Array& args) {
    client_.async_call("nvim_execute_lua", code, args);
}

void Nvim::async_call_function(const std::string& fn, const Array& args) {
    client_.async_call("nvim_call_function", fn, args);
}

void Nvim::async_call_dict_function(const Object& dict, const std::string& fn, const Array& args) {
    client_.async_call("nvim_call_dict_function", dict, fn, args);
}

void Nvim::async_strwidth(const std::string& text) {
    client_.async_call("nvim_strwidth", text);
}

void Nvim::async_list_runtime_paths() {
    client_.async_call("nvim_list_runtime_paths");
}

void Nvim::async_set_current_dir(const std::string& dir) {
    client_.async_call("nvim_set_current_dir", dir);
}

void Nvim::async_get_current_line() {
    client_.async_call("nvim_get_current_line");
}

void Nvim::async_set_current_line(const std::string& line) {
    client_.async_call("nvim_set_current_line", line);
}

void Nvim::async_del_current_line() {
    client_.async_call("nvim_del_current_line");
}

void Nvim::async_get_var(const std::string& name) {
    client_.async_call("nvim_get_var", name);
}

void Nvim::async_set_var(const std::string& name, const Object& value) {
    client_.async_call("nvim_set_var", name, value);
}

void Nvim::async_del_var(const std::string& name) {
    client_.async_call("nvim_del_var", name);
}

void Nvim::async_get_vvar(const std::string& name) {
    client_.async_call("nvim_get_vvar", name);
}

void Nvim::async_set_vvar(const std::string& name, const Object& value) {
    client_.async_call("nvim_set_vvar", name, value);
}

void Nvim::async_get_option(const std::string& name) {
    client_.async_call("nvim_get_option", name);
}

void Nvim::async_set_option(const std::string& name, const Object& value) {
    client_.async_call("nvim_set_option", name, value);
}

void Nvim::async_out_write(const std::string& str) {
    client_.async_call("nvim_out_write", str);
}

void Nvim::async_err_write(const std::string& str) {
    client_.async_call("nvim_err_write", str);
}

void Nvim::async_err_writeln(const std::string& str) {
    client_.async_call("nvim_err_writeln", str);
}

void Nvim::async_list_bufs() {
    client_.async_call("nvim_list_bufs");
}

void Nvim::async_get_current_buf() {
    client_.async_call("nvim_get_current_buf");
}

void Nvim::async_set_current_buf(Buffer buffer) {
    client_.async_call("nvim_set_current_buf", buffer);
}

void Nvim::async_list_wins() {
    client_.async_call("nvim_list_wins");
}

void Nvim::async_get_current_win() {
    client_.async_call("nvim_get_current_win");
}

void Nvim::async_set_current_win(Window window) {
    client_.async_call("nvim_set_current_win", window);
}

void Nvim::async_create_buf(bool listed, bool scratch) {
    client_.async_call("nvim_create_buf", listed, scratch);
}

void Nvim::async_open_win(Buffer buffer, bool enter, const Dictionary& config) {
    client_.async_call("nvim_open_win", buffer, enter, config);
}

void Nvim::async_list_tabpages() {
    client_.async_call("nvim_list_tabpages");
}

void Nvim::async_get_current_tabpage() {
    client_.async_call("nvim_get_current_tabpage");
}

void Nvim::async_set_current_tabpage(Tabpage tabpage) {
    client_.async_call("nvim_set_current_tabpage", tabpage);
}

void Nvim::async_create_namespace(const std::string& name) {
    client_.async_call("nvim_create_namespace", name);
}

void Nvim::async_get_namespaces() {
    client_.async_call("nvim_get_namespaces");
}

void Nvim::async_paste(const std::string& data, bool crlf, Integer phase) {
    client_.async_call("nvim_paste", data, crlf, phase);
}

void Nvim::async_put(const std::vector<std::string>& lines, const std::string& type, bool after, bool follow) {
    client_.async_call("nvim_put", lines, type, after, follow);
}

void Nvim::async_subscribe(const std::string& event) {
    client_.async_call("nvim_subscribe", event);
}

void Nvim::async_unsubscribe(const std::string& event) {
    client_.async_call("nvim_unsubscribe", event);
}

void Nvim::async_get_color_by_name(const std::string& name) {
    client_.async_call("nvim_get_color_by_name", name);
}

void Nvim::async_get_color_map() {
    client_.async_call("nvim_get_color_map");
}

void Nvim::async_get_context(const Dictionary& opts) {
    client_.async_call("nvim_get_context", opts);
}

void Nvim::async_load_context(const Dictionary& dict) {
    client_.async_call("nvim_load_context", dict);
}

void Nvim::async_get_mode() {
    client_.async_call("nvim_get_mode");
}

void Nvim::async_get_keymap(const std::string& mode) {
    client_.async_call("nvim_get_keymap", mode);
}

void Nvim::async_set_keymap(const std::string& mode, const std::string& lhs, const std::string& rhs, const Dictionary& opts) {
    client_.async_call("nvim_set_keymap", mode, lhs, rhs, opts);
}

void Nvim::async_del_keymap(const std::string& mode, const std::string& lhs) {
    client_.async_call("nvim_del_keymap", mode, lhs);
}

void Nvim::async_get_commands(const Dictionary& opts) {
    client_.async_call("nvim_get_commands", opts);
}

void Nvim::async_get_api_info() {
    client_.async_call("nvim_get_api_info");
}

void Nvim::async_set_client_info(const std::string& name, const Dictionary& version, const std::string& type, const Dictionary& methods, const Dictionary& attributes) {
    client_.async_call("nvim_set_client_info", name, version, type, methods, attributes);
}

void Nvim::async_get_chan_info(Integer chan) {
    client_.async_call("nvim_get_chan_info", chan);
}

void Nvim::async_list_chans() {
    client_.async_call("nvim_list_chans");
}

void Nvim::async_call_atomic(const Array& calls) {
    client_.async_call("nvim_call_atomic", calls);
}

void Nvim::async_parse_expression(const std::string& expr, const std::string& flags, bool highlight) {
    client_.async_call("nvim_parse_expression", expr, flags, highlight);
}

void Nvim::async_list_uis() {
    client_.async_call("nvim_list_uis");
}

void Nvim::async_get_proc_children(Integer pid) {
    client_.async_call("nvim_get_proc_children", pid);
}

void Nvim::async_get_proc(Integer pid) {
    client_.async_call("nvim_get_proc", pid);
}

void Nvim::async_select_popupmenu_item(Integer item, bool insert, bool finish, const Dictionary& opts) {
    client_.async_call("nvim_select_popupmenu_item", item, insert, finish, opts);
}

void Nvim::async_win_get_buf(Window window) {
    client_.async_call("nvim_win_get_buf", window);
}

void Nvim::async_win_set_buf(Window window, Buffer buffer) {
    client_.async_call("nvim_win_set_buf", window, buffer);
}

void Nvim::async_win_get_cursor(Window window) {
    client_.async_call("nvim_win_get_cursor", window);
}

void Nvim::async_win_set_cursor(Window window, const std::vector<Integer>& pos) {
    client_.async_call("nvim_win_set_cursor", window, pos);
}

void Nvim::async_win_get_height(Window window) {
    client_.async_call("nvim_win_get_height", window);
}

void Nvim::async_win_set_height(Window window, Integer height) {
    client_.async_call("nvim_win_set_height", window, height);
}

void Nvim::async_win_get_width(Window window) {
    client_.async_call("nvim_win_get_width", window);
}

void Nvim::async_win_set_width(Window window, Integer width) {
    client_.async_call("nvim_win_set_width", window, width);
}

void Nvim::async_win_get_var(Window window, const std::string& name) {
    client_.async_call("nvim_win_get_var", window, name);
}

void Nvim::async_win_set_var(Window window, const std::string& name, const Object& value) {
    client_.async_call("nvim_win_set_var", window, name, value);
}

void Nvim::async_win_del_var(Window window, const std::string& name) {
    client_.async_call("nvim_win_del_var", window, name);
}

void Nvim::async_win_get_option(Window window, const std::string& name) {
    client_.async_call("nvim_win_get_option", window, name);
}

void Nvim::async_win_set_option(Window window, const std::string& name, const Object& value) {
    client_.async_call("nvim_win_set_option", window, name, value);
}

void Nvim::async_win_get_position(Window window) {
    client_.async_call("nvim_win_get_position", window);
}

void Nvim::async_win_get_tabpage(Window window) {
    client_.async_call("nvim_win_get_tabpage", window);
}

void Nvim::async_win_get_number(Window window) {
    client_.async_call("nvim_win_get_number", window);
}

void Nvim::async_win_is_valid(Window window) {
    client_.async_call("nvim_win_is_valid", window);
}

void Nvim::async_win_set_config(Window window, const Dictionary& config) {
    client_.async_call("nvim_win_set_config", window, config);
}

void Nvim::async_win_get_config(Window window) {
    client_.async_call("nvim_win_get_config", window);
}

void Nvim::async_win_close(Window window, bool force) {
    client_.async_call("nvim_win_close", window, force);
}

} //namespace nvim