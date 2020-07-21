#ifndef NVIM_CPP_TREE
#define NVIM_CPP_TREE

#include <list>
#include <tuple>
#include <unordered_map>
#include <boost/filesystem.hpp>
#include "column.h"
#include "nvim.hpp"

#ifdef NDEBUG
#define INFO(...)
#else
#define INFO(...) \
do{ \
    fprintf(stdout, "[INFO]%s %s(Line %d): ",__FILE__,__FUNCTION__,__LINE__); \
    fprintf(stdout, __VA_ARGS__); \
}while(0)
#endif

using std::list;
using std::string;
using std::vector;
using std::unordered_map;
namespace tree {
// TODO using Hash = std::unordered_map<class _Key, class _Tp>;
class Tree
{
public:
    Tree() = delete; // delete default constructor
    ~Tree();
    Tree(int bufnr, int icon_ns_id);
    enum ClipboardMode {COPY, MOVE};
    static ClipboardMode paste_mode;
    static list<string> clipboard;
	static nvim::Nvim *api;
    int bufnr = -1;
    int icon_ns_id = -1;
    Config cfg;
    Context ctx;
    string input;

    void handleNewFile(const string &input);
    void handleRename(string &input);
    void toggle_select(const nvim::Array &args);
    void toggle_select_all(const nvim::Array &args);
    void execute_system(const nvim::Array &args);
    void redraw_line(int sl, int el);
    void changeRoot(const string &path);
    void action(const string &action, const nvim::Array &args, const Map &context);

    void remove();
    void move(const nvim::Array &args);
    Map get_candidate(const int pos);
    void new_file(const nvim::Array &args);
    void redraw(const nvim::Array &args);
    void redraw_recursively(int l);
    void yank_path(const nvim::Array &args);
    void pre_remove(const nvim::Array &args);
    void print(const nvim::Array &args);
    void debug(const nvim::Array &args);
    void open_tree(const nvim::Array &args);
    void open_or_close_tree_recursively(const nvim::Array &args);
    void open(const nvim::Array &args);
    void drop(const nvim::Array &args);
    void call(const nvim::Array &args);
    void copy_(const nvim::Array &args);
    void _copy_or_move(const nvim::Array &args);
    void rename(const nvim::Array &args);
    void expandRecursively(const FileItem &item, vector<FileItem*> &fileitem_lst);
    void cd(const nvim::Array &args);
    void goto_(const nvim::Array &args);
    void toggle_ignored_files(const nvim::Array &args);
    void paste(const int ln, const string &src, const string &dest);
    void pre_paste(const nvim::Array &args);
    void view(const nvim::Array &args);

    inline void buf_set_lines(int s, int e, bool strict, const vector<string> &replacement)
    {
        api->async_buf_set_option(bufnr, "modifiable", true);
        api->async_buf_set_lines(bufnr, s, e, strict, replacement);
        api->async_buf_set_option(bufnr, "modifiable", false);
        // TODO: Fine-grained targets update
        collect_targets();
    };

private:
    vector<FileItem *> m_fileitem;
    unordered_map<int, vector<Cell>> col_map;
    unordered_map<string, bool> expandStore;
    unordered_map<string, int> cursorHistory;
    list<int> targets;
    void hline(int sl, int el);
    int find_parent(int l);
    std::tuple<int, int> find_range(int l);
    void set_cursor();
    void insert_entrylist(const vector<FileItem*> &, const int pos, vector<string>& ret);
    void insert_item(const int pos);
    void _toggle_select(const int pos);
    void collect_targets();
    void insert_rootcell(const int pos);
    void erase_entrylist(const int s, const int e);
    string makeline(const int pos);
    void entryInfoListRecursively(const FileItem &, vector<FileItem*>& fileitem_lst);

    void save_cursor();
    void shrinkRecursively(const string &path);
    void expandRecursively(const FileItem&, list<FileItem*> &fileitem_lst);
};

} // namespace tree
#endif
