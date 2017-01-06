// Auto generated

#ifndef NEOVIM_CPP__NVIM_HPP_
#define NEOVIM_CPP__NVIm_HPP_

#include "nvim_rpc.hpp"

class Nvim {
public:

    Integer nvim_buf_line_count (Buffer buffer) { 
        
        Integer res;
        client.send("nvim_buf_line_count", res, buffer); 
        return res;
        
    }

    std::string buffer_get_line (Buffer buffer, Integer index) { 
        
        std::string res;
        client.send("buffer_get_line", res, buffer, index); 
        return res;
        
    }

    void buffer_set_line (Buffer buffer, Integer index, const std::string& line) { 
        
        client.send("buffer_set_line", nullptr, buffer, index, line); 
        
    }

    void buffer_del_line (Buffer buffer, Integer index) { 
        
        client.send("buffer_del_line", nullptr, buffer, index); 
        
    }

    std::vector<std::string> buffer_get_line_slice (Buffer buffer, Integer start, Integer end, bool include_start, bool include_end) { 
        
        std::vector<std::string> res;
        client.send("buffer_get_line_slice", res, buffer, start, end, include_start, include_end); 
        return res;
        
    }

    std::vector<std::string> nvim_buf_get_lines (Buffer buffer, Integer start, Integer end, bool strict_indexing) { 
        
        std::vector<std::string> res;
        client.send("nvim_buf_get_lines", res, buffer, start, end, strict_indexing); 
        return res;
        
    }

    void buffer_set_line_slice (Buffer buffer, Integer start, Integer end, bool include_start, bool include_end, const std::vector<std::string>& replacement) { 
        
        client.send("buffer_set_line_slice", nullptr, buffer, start, end, include_start, include_end, replacement); 
        
    }

    void nvim_buf_set_lines (Buffer buffer, Integer start, Integer end, bool strict_indexing, const std::vector<std::string>& replacement) { 
        
        client.send("nvim_buf_set_lines", nullptr, buffer, start, end, strict_indexing, replacement); 
        
    }

    Object nvim_buf_get_var (Buffer buffer, const std::string& name) { 
        
        Object res;
        client.send("nvim_buf_get_var", res, buffer, name); 
        return res;
        
    }

    void nvim_buf_set_var (Buffer buffer, const std::string& name, const Object& value) { 
        
        client.send("nvim_buf_set_var", nullptr, buffer, name, value); 
        
    }

    void nvim_buf_del_var (Buffer buffer, const std::string& name) { 
        
        client.send("nvim_buf_del_var", nullptr, buffer, name); 
        
    }

    Object buffer_set_var (Buffer buffer, const std::string& name, const Object& value) { 
        
        Object res;
        client.send("buffer_set_var", res, buffer, name, value); 
        return res;
        
    }

    Object buffer_del_var (Buffer buffer, const std::string& name) { 
        
        Object res;
        client.send("buffer_del_var", res, buffer, name); 
        return res;
        
    }

    Object nvim_buf_get_option (Buffer buffer, const std::string& name) { 
        
        Object res;
        client.send("nvim_buf_get_option", res, buffer, name); 
        return res;
        
    }

    void nvim_buf_set_option (Buffer buffer, const std::string& name, const Object& value) { 
        
        client.send("nvim_buf_set_option", nullptr, buffer, name, value); 
        
    }

    Integer nvim_buf_get_number (Buffer buffer) { 
        
        Integer res;
        client.send("nvim_buf_get_number", res, buffer); 
        return res;
        
    }

    std::string nvim_buf_get_name (Buffer buffer) { 
        
        std::string res;
        client.send("nvim_buf_get_name", res, buffer); 
        return res;
        
    }

    void nvim_buf_set_name (Buffer buffer, const std::string& name) { 
        
        client.send("nvim_buf_set_name", nullptr, buffer, name); 
        
    }

    bool nvim_buf_is_valid (Buffer buffer) { 
        
        bool res;
        client.send("nvim_buf_is_valid", res, buffer); 
        return res;
        
    }

    void buffer_insert (Buffer buffer, Integer lnum, const std::vector<std::string>& lines) { 
        
        client.send("buffer_insert", nullptr, buffer, lnum, lines); 
        
    }

    std::vector<Integer> nvim_buf_get_mark (Buffer buffer, const std::string& name) { 
        
        std::vector<Integer> res;
        client.send("nvim_buf_get_mark", res, buffer, name); 
        return res;
        
    }

    Integer nvim_buf_add_highlight (Buffer buffer, Integer src_id, const std::string& hl_group, Integer line, Integer col_start, Integer col_end) { 
        
        Integer res;
        client.send("nvim_buf_add_highlight", res, buffer, src_id, hl_group, line, col_start, col_end); 
        return res;
        
    }

    void nvim_buf_clear_highlight (Buffer buffer, Integer src_id, Integer line_start, Integer line_end) { 
        
        client.send("nvim_buf_clear_highlight", nullptr, buffer, src_id, line_start, line_end); 
        
    }

    std::vector<Window> nvim_tabpage_list_wins (Tabpage tabpage) { 
        
        std::vector<Window> res;
        client.send("nvim_tabpage_list_wins", res, tabpage); 
        return res;
        
    }

    Object nvim_tabpage_get_var (Tabpage tabpage, const std::string& name) { 
        
        Object res;
        client.send("nvim_tabpage_get_var", res, tabpage, name); 
        return res;
        
    }

    void nvim_tabpage_set_var (Tabpage tabpage, const std::string& name, const Object& value) { 
        
        client.send("nvim_tabpage_set_var", nullptr, tabpage, name, value); 
        
    }

    void nvim_tabpage_del_var (Tabpage tabpage, const std::string& name) { 
        
        client.send("nvim_tabpage_del_var", nullptr, tabpage, name); 
        
    }

    Object tabpage_set_var (Tabpage tabpage, const std::string& name, const Object& value) { 
        
        Object res;
        client.send("tabpage_set_var", res, tabpage, name, value); 
        return res;
        
    }

    Object tabpage_del_var (Tabpage tabpage, const std::string& name) { 
        
        Object res;
        client.send("tabpage_del_var", res, tabpage, name); 
        return res;
        
    }

    Window nvim_tabpage_get_win (Tabpage tabpage) { 
        
        Window res;
        client.send("nvim_tabpage_get_win", res, tabpage); 
        return res;
        
    }

    Integer nvim_tabpage_get_number (Tabpage tabpage) { 
        
        Integer res;
        client.send("nvim_tabpage_get_number", res, tabpage); 
        return res;
        
    }

    bool nvim_tabpage_is_valid (Tabpage tabpage) { 
        
        bool res;
        client.send("nvim_tabpage_is_valid", res, tabpage); 
        return res;
        
    }

    void ui_attach (Integer width, Integer height, bool enable_rgb) { 
        
        client.send("ui_attach", nullptr, width, height, enable_rgb); 
        
    }

    void nvim_ui_detach () { 
        
        client.send("nvim_ui_detach", nullptr); 
        
    }

    void nvim_ui_try_resize (Integer width, Integer height) { 
        
        client.send("nvim_ui_try_resize", nullptr, width, height); 
        
    }

    void nvim_ui_set_option (const std::string& name, const Object& value) { 
        
        client.send("nvim_ui_set_option", nullptr, name, value); 
        
    }

    void nvim_command (const std::string& command) { 
        
        client.send("nvim_command", nullptr, command); 
        
    }

    void nvim_feedkeys (const std::string& keys, const std::string& mode, bool escape_csi) { 
        
        client.send("nvim_feedkeys", nullptr, keys, mode, escape_csi); 
        
    }

    Integer nvim_input (const std::string& keys) { 
        
        Integer res;
        client.send("nvim_input", res, keys); 
        return res;
        
    }

    std::string nvim_replace_termcodes (const std::string& str, bool from_part, bool do_lt, bool special) { 
        
        std::string res;
        client.send("nvim_replace_termcodes", res, str, from_part, do_lt, special); 
        return res;
        
    }

    std::string nvim_command_output (const std::string& str) { 
        
        std::string res;
        client.send("nvim_command_output", res, str); 
        return res;
        
    }

    Object nvim_eval (const std::string& expr) { 
        
        Object res;
        client.send("nvim_eval", res, expr); 
        return res;
        
    }

    Integer nvim_strwidth (const std::string& str) { 
        
        Integer res;
        client.send("nvim_strwidth", res, str); 
        return res;
        
    }

    std::vector<std::string> nvim_list_runtime_paths () { 
        
        std::vector<std::string> res;
        client.send("nvim_list_runtime_paths", res); 
        return res;
        
    }

    void nvim_set_current_dir (const std::string& dir) { 
        
        client.send("nvim_set_current_dir", nullptr, dir); 
        
    }

    std::string nvim_get_current_line () { 
        
        std::string res;
        client.send("nvim_get_current_line", res); 
        return res;
        
    }

    void nvim_set_current_line (const std::string& line) { 
        
        client.send("nvim_set_current_line", nullptr, line); 
        
    }

    void nvim_del_current_line () { 
        
        client.send("nvim_del_current_line", nullptr); 
        
    }

    Object nvim_get_var (const std::string& name) { 
        
        Object res;
        client.send("nvim_get_var", res, name); 
        return res;
        
    }

    void nvim_set_var (const std::string& name, const Object& value) { 
        
        client.send("nvim_set_var", nullptr, name, value); 
        
    }

    void nvim_del_var (const std::string& name) { 
        
        client.send("nvim_del_var", nullptr, name); 
        
    }

    Object vim_set_var (const std::string& name, const Object& value) { 
        
        Object res;
        client.send("vim_set_var", res, name, value); 
        return res;
        
    }

    Object vim_del_var (const std::string& name) { 
        
        Object res;
        client.send("vim_del_var", res, name); 
        return res;
        
    }

    Object nvim_get_vvar (const std::string& name) { 
        
        Object res;
        client.send("nvim_get_vvar", res, name); 
        return res;
        
    }

    Object nvim_get_option (const std::string& name) { 
        
        Object res;
        client.send("nvim_get_option", res, name); 
        return res;
        
    }

    void nvim_set_option (const std::string& name, const Object& value) { 
        
        client.send("nvim_set_option", nullptr, name, value); 
        
    }

    void nvim_out_write (const std::string& str) { 
        
        client.send("nvim_out_write", nullptr, str); 
        
    }

    void nvim_err_write (const std::string& str) { 
        
        client.send("nvim_err_write", nullptr, str); 
        
    }

    void nvim_err_writeln (const std::string& str) { 
        
        client.send("nvim_err_writeln", nullptr, str); 
        
    }

    std::vector<Buffer> nvim_list_bufs () { 
        
        std::vector<Buffer> res;
        client.send("nvim_list_bufs", res); 
        return res;
        
    }

    Buffer nvim_get_current_buf () { 
        
        Buffer res;
        client.send("nvim_get_current_buf", res); 
        return res;
        
    }

    void nvim_set_current_buf (Buffer buffer) { 
        
        client.send("nvim_set_current_buf", nullptr, buffer); 
        
    }

    std::vector<Window> nvim_list_wins () { 
        
        std::vector<Window> res;
        client.send("nvim_list_wins", res); 
        return res;
        
    }

    Window nvim_get_current_win () { 
        
        Window res;
        client.send("nvim_get_current_win", res); 
        return res;
        
    }

    void nvim_set_current_win (Window window) { 
        
        client.send("nvim_set_current_win", nullptr, window); 
        
    }

    std::vector<Tabpage> nvim_list_tabpages () { 
        
        std::vector<Tabpage> res;
        client.send("nvim_list_tabpages", res); 
        return res;
        
    }

    Tabpage nvim_get_current_tabpage () { 
        
        Tabpage res;
        client.send("nvim_get_current_tabpage", res); 
        return res;
        
    }

    void nvim_set_current_tabpage (Tabpage tabpage) { 
        
        client.send("nvim_set_current_tabpage", nullptr, tabpage); 
        
    }

    void nvim_subscribe (const std::string& event) { 
        
        client.send("nvim_subscribe", nullptr, event); 
        
    }

    void nvim_unsubscribe (const std::string& event) { 
        
        client.send("nvim_unsubscribe", nullptr, event); 
        
    }

    Integer nvim_get_color_by_name (const std::string& name) { 
        
        Integer res;
        client.send("nvim_get_color_by_name", res, name); 
        return res;
        
    }

    Buffer nvim_win_get_buf (Window window) { 
        
        Buffer res;
        client.send("nvim_win_get_buf", res, window); 
        return res;
        
    }

    std::vector<Integer> nvim_win_get_cursor (Window window) { 
        
        std::vector<Integer> res;
        client.send("nvim_win_get_cursor", res, window); 
        return res;
        
    }

    void nvim_win_set_cursor (Window window, const std::vector<Integer>& pos) { 
        
        client.send("nvim_win_set_cursor", nullptr, window, pos); 
        
    }

    Integer nvim_win_get_height (Window window) { 
        
        Integer res;
        client.send("nvim_win_get_height", res, window); 
        return res;
        
    }

    void nvim_win_set_height (Window window, Integer height) { 
        
        client.send("nvim_win_set_height", nullptr, window, height); 
        
    }

    Integer nvim_win_get_width (Window window) { 
        
        Integer res;
        client.send("nvim_win_get_width", res, window); 
        return res;
        
    }

    void nvim_win_set_width (Window window, Integer width) { 
        
        client.send("nvim_win_set_width", nullptr, window, width); 
        
    }

    Object nvim_win_get_var (Window window, const std::string& name) { 
        
        Object res;
        client.send("nvim_win_get_var", res, window, name); 
        return res;
        
    }

    void nvim_win_set_var (Window window, const std::string& name, const Object& value) { 
        
        client.send("nvim_win_set_var", nullptr, window, name, value); 
        
    }

    void nvim_win_del_var (Window window, const std::string& name) { 
        
        client.send("nvim_win_del_var", nullptr, window, name); 
        
    }

    Object window_set_var (Window window, const std::string& name, const Object& value) { 
        
        Object res;
        client.send("window_set_var", res, window, name, value); 
        return res;
        
    }

    Object window_del_var (Window window, const std::string& name) { 
        
        Object res;
        client.send("window_del_var", res, window, name); 
        return res;
        
    }

    Object nvim_win_get_option (Window window, const std::string& name) { 
        
        Object res;
        client.send("nvim_win_get_option", res, window, name); 
        return res;
        
    }

    void nvim_win_set_option (Window window, const std::string& name, const Object& value) { 
        
        client.send("nvim_win_set_option", nullptr, window, name, value); 
        
    }

    std::vector<Integer> nvim_win_get_position (Window window) { 
        
        std::vector<Integer> res;
        client.send("nvim_win_get_position", res, window); 
        return res;
        
    }

    Tabpage nvim_win_get_tabpage (Window window) { 
        
        Tabpage res;
        client.send("nvim_win_get_tabpage", res, window); 
        return res;
        
    }

    Integer nvim_win_get_number (Window window) { 
        
        Integer res;
        client.send("nvim_win_get_number", res, window); 
        return res;
        
    }

    bool nvim_win_is_valid (Window window) { 
        
        bool res;
        client.send("nvim_win_is_valid", res, window); 
        return res;
        
    }

    Integer buffer_line_count (Buffer buffer) { 
        
        Integer res;
        client.send("buffer_line_count", res, buffer); 
        return res;
        
    }

    std::vector<std::string> buffer_get_lines (Buffer buffer, Integer start, Integer end, bool strict_indexing) { 
        
        std::vector<std::string> res;
        client.send("buffer_get_lines", res, buffer, start, end, strict_indexing); 
        return res;
        
    }

    void buffer_set_lines (Buffer buffer, Integer start, Integer end, bool strict_indexing, const std::vector<std::string>& replacement) { 
        
        client.send("buffer_set_lines", nullptr, buffer, start, end, strict_indexing, replacement); 
        
    }

    Object buffer_get_var (Buffer buffer, const std::string& name) { 
        
        Object res;
        client.send("buffer_get_var", res, buffer, name); 
        return res;
        
    }

    Object buffer_get_option (Buffer buffer, const std::string& name) { 
        
        Object res;
        client.send("buffer_get_option", res, buffer, name); 
        return res;
        
    }

    void buffer_set_option (Buffer buffer, const std::string& name, const Object& value) { 
        
        client.send("buffer_set_option", nullptr, buffer, name, value); 
        
    }

    Integer buffer_get_number (Buffer buffer) { 
        
        Integer res;
        client.send("buffer_get_number", res, buffer); 
        return res;
        
    }

    std::string buffer_get_name (Buffer buffer) { 
        
        std::string res;
        client.send("buffer_get_name", res, buffer); 
        return res;
        
    }

    void buffer_set_name (Buffer buffer, const std::string& name) { 
        
        client.send("buffer_set_name", nullptr, buffer, name); 
        
    }

    bool buffer_is_valid (Buffer buffer) { 
        
        bool res;
        client.send("buffer_is_valid", res, buffer); 
        return res;
        
    }

    std::vector<Integer> buffer_get_mark (Buffer buffer, const std::string& name) { 
        
        std::vector<Integer> res;
        client.send("buffer_get_mark", res, buffer, name); 
        return res;
        
    }

    Integer buffer_add_highlight (Buffer buffer, Integer src_id, const std::string& hl_group, Integer line, Integer col_start, Integer col_end) { 
        
        Integer res;
        client.send("buffer_add_highlight", res, buffer, src_id, hl_group, line, col_start, col_end); 
        return res;
        
    }

    void buffer_clear_highlight (Buffer buffer, Integer src_id, Integer line_start, Integer line_end) { 
        
        client.send("buffer_clear_highlight", nullptr, buffer, src_id, line_start, line_end); 
        
    }

    std::vector<Window> tabpage_get_windows (Tabpage tabpage) { 
        
        std::vector<Window> res;
        client.send("tabpage_get_windows", res, tabpage); 
        return res;
        
    }

    Object tabpage_get_var (Tabpage tabpage, const std::string& name) { 
        
        Object res;
        client.send("tabpage_get_var", res, tabpage, name); 
        return res;
        
    }

    Window tabpage_get_window (Tabpage tabpage) { 
        
        Window res;
        client.send("tabpage_get_window", res, tabpage); 
        return res;
        
    }

    bool tabpage_is_valid (Tabpage tabpage) { 
        
        bool res;
        client.send("tabpage_is_valid", res, tabpage); 
        return res;
        
    }

    void ui_detach () { 
        
        client.send("ui_detach", nullptr); 
        
    }

    Object ui_try_resize (Integer width, Integer height) { 
        
        Object res;
        client.send("ui_try_resize", res, width, height); 
        return res;
        
    }

    void vim_command (const std::string& command) { 
        
        client.send("vim_command", nullptr, command); 
        
    }

    void vim_feedkeys (const std::string& keys, const std::string& mode, bool escape_csi) { 
        
        client.send("vim_feedkeys", nullptr, keys, mode, escape_csi); 
        
    }

    Integer vim_input (const std::string& keys) { 
        
        Integer res;
        client.send("vim_input", res, keys); 
        return res;
        
    }

    std::string vim_replace_termcodes (const std::string& str, bool from_part, bool do_lt, bool special) { 
        
        std::string res;
        client.send("vim_replace_termcodes", res, str, from_part, do_lt, special); 
        return res;
        
    }

    std::string vim_command_output (const std::string& str) { 
        
        std::string res;
        client.send("vim_command_output", res, str); 
        return res;
        
    }

    Object vim_eval (const std::string& expr) { 
        
        Object res;
        client.send("vim_eval", res, expr); 
        return res;
        
    }

    Integer vim_strwidth (const std::string& str) { 
        
        Integer res;
        client.send("vim_strwidth", res, str); 
        return res;
        
    }

    std::vector<std::string> vim_list_runtime_paths () { 
        
        std::vector<std::string> res;
        client.send("vim_list_runtime_paths", res); 
        return res;
        
    }

    void vim_change_directory (const std::string& dir) { 
        
        client.send("vim_change_directory", nullptr, dir); 
        
    }

    std::string vim_get_current_line () { 
        
        std::string res;
        client.send("vim_get_current_line", res); 
        return res;
        
    }

    void vim_set_current_line (const std::string& line) { 
        
        client.send("vim_set_current_line", nullptr, line); 
        
    }

    void vim_del_current_line () { 
        
        client.send("vim_del_current_line", nullptr); 
        
    }

    Object vim_get_var (const std::string& name) { 
        
        Object res;
        client.send("vim_get_var", res, name); 
        return res;
        
    }

    Object vim_get_vvar (const std::string& name) { 
        
        Object res;
        client.send("vim_get_vvar", res, name); 
        return res;
        
    }

    Object vim_get_option (const std::string& name) { 
        
        Object res;
        client.send("vim_get_option", res, name); 
        return res;
        
    }

    void vim_set_option (const std::string& name, const Object& value) { 
        
        client.send("vim_set_option", nullptr, name, value); 
        
    }

    void vim_out_write (const std::string& str) { 
        
        client.send("vim_out_write", nullptr, str); 
        
    }

    void vim_err_write (const std::string& str) { 
        
        client.send("vim_err_write", nullptr, str); 
        
    }

    void vim_report_error (const std::string& str) { 
        
        client.send("vim_report_error", nullptr, str); 
        
    }

    std::vector<Buffer> vim_get_buffers () { 
        
        std::vector<Buffer> res;
        client.send("vim_get_buffers", res); 
        return res;
        
    }

    Buffer vim_get_current_buffer () { 
        
        Buffer res;
        client.send("vim_get_current_buffer", res); 
        return res;
        
    }

    void vim_set_current_buffer (Buffer buffer) { 
        
        client.send("vim_set_current_buffer", nullptr, buffer); 
        
    }

    std::vector<Window> vim_get_windows () { 
        
        std::vector<Window> res;
        client.send("vim_get_windows", res); 
        return res;
        
    }

    Window vim_get_current_window () { 
        
        Window res;
        client.send("vim_get_current_window", res); 
        return res;
        
    }

    void vim_set_current_window (Window window) { 
        
        client.send("vim_set_current_window", nullptr, window); 
        
    }

    std::vector<Tabpage> vim_get_tabpages () { 
        
        std::vector<Tabpage> res;
        client.send("vim_get_tabpages", res); 
        return res;
        
    }

    Tabpage vim_get_current_tabpage () { 
        
        Tabpage res;
        client.send("vim_get_current_tabpage", res); 
        return res;
        
    }

    void vim_set_current_tabpage (Tabpage tabpage) { 
        
        client.send("vim_set_current_tabpage", nullptr, tabpage); 
        
    }

    void vim_subscribe (const std::string& event) { 
        
        client.send("vim_subscribe", nullptr, event); 
        
    }

    void vim_unsubscribe (const std::string& event) { 
        
        client.send("vim_unsubscribe", nullptr, event); 
        
    }

    Integer vim_name_to_color (const std::string& name) { 
        
        Integer res;
        client.send("vim_name_to_color", res, name); 
        return res;
        
    }

    Buffer window_get_buffer (Window window) { 
        
        Buffer res;
        client.send("window_get_buffer", res, window); 
        return res;
        
    }

    std::vector<Integer> window_get_cursor (Window window) { 
        
        std::vector<Integer> res;
        client.send("window_get_cursor", res, window); 
        return res;
        
    }

    void window_set_cursor (Window window, const std::vector<Integer>& pos) { 
        
        client.send("window_set_cursor", nullptr, window, pos); 
        
    }

    Integer window_get_height (Window window) { 
        
        Integer res;
        client.send("window_get_height", res, window); 
        return res;
        
    }

    void window_set_height (Window window, Integer height) { 
        
        client.send("window_set_height", nullptr, window, height); 
        
    }

    Integer window_get_width (Window window) { 
        
        Integer res;
        client.send("window_get_width", res, window); 
        return res;
        
    }

    void window_set_width (Window window, Integer width) { 
        
        client.send("window_set_width", nullptr, window, width); 
        
    }

    Object window_get_var (Window window, const std::string& name) { 
        
        Object res;
        client.send("window_get_var", res, window, name); 
        return res;
        
    }

    Object window_get_option (Window window, const std::string& name) { 
        
        Object res;
        client.send("window_get_option", res, window, name); 
        return res;
        
    }

    void window_set_option (Window window, const std::string& name, const Object& value) { 
        
        client.send("window_set_option", nullptr, window, name, value); 
        
    }

    std::vector<Integer> window_get_position (Window window) { 
        
        std::vector<Integer> res;
        client.send("window_get_position", res, window); 
        return res;
        
    }

    Tabpage window_get_tabpage (Window window) { 
        
        Tabpage res;
        client.send("window_get_tabpage", res, window); 
        return res;
        
    }

    bool window_is_valid (Window window) { 
        
        bool res;
        client.send("window_is_valid", res, window); 
        return res;
        
    }


private:
    NvimRPC client;

};

#endif //NEOVIM_CPP__NVIM_HPP_