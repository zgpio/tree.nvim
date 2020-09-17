#ifndef NVIM_CPP_TREE
#define NVIM_CPP_TREE

#include <boost/filesystem.hpp>
#include <list>
#include <tuple>
#include <unordered_map>
#include "column.h"
#include "nvim.hpp"

using std::list;
using std::string;
using std::unordered_map;
using std::vector;
namespace tree
{
// TODO using Hash = std::unordered_map<class _Key, class _Tp>;
class Tree
{
   public:
    typedef nvim::Array ArgType;
    Tree() = delete;  // delete default constructor
    ~Tree();
    Tree(int bufnr, int ns_id);
    enum ClipboardMode { COPY, MOVE };
    static ClipboardMode paste_mode;
    static list<string> clipboard;
    static nvim::Nvim *api;
    int bufnr = -1;
    int ns_id = -1;
    Config cfg;
    Context ctx;
    string input;

    void handleNewFile(const string &input);
    void handleRename(string &input);
    void toggle_select(const ArgType &args);
    void toggle_select_all(const ArgType &args);
    void execute_system(const ArgType &args);
    void redraw_line(int sl, int el);
    void changeRoot(const string &path);
    void action(const string &action, const ArgType &args, const Map &context);

    void remove();
    void move(const ArgType &args);
    Map get_candidate(const int pos);
    void new_file(const ArgType &args);
    void redraw(const ArgType &args);
    void redraw_recursively(int l);
    void yank_path(const ArgType &args);
    void pre_remove(const ArgType &args);
    void print(const ArgType &args);
    void debug(const ArgType &args);
    void open_tree(const ArgType &args);
    void open_or_close_tree_recursively(const ArgType &args);
    void open(const ArgType &args);
    void drop(const ArgType &args);
    void call(const ArgType &args);
    void copy_(const ArgType &args);
    void _copy_or_move(const ArgType &args);
    void rename(const ArgType &args);
    void expandRecursively(const FileItem &item, vector<FileItem *> &fileitem_lst);
    void cd(const ArgType &args);
    void goto_(const ArgType &args);
    void toggle_ignored_files(const ArgType &args);
    void paste(const int ln, const string &src, const string &dest);
    void pre_paste(const ArgType &args);
    void view(const ArgType &args);

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
    vector<Cell> col_map[COLUMN_NR];
    unordered_map<string, bool> expandStore;
    unordered_map<string, int> cursorHistory;
    list<int> m_targets;
    void hline(int sl, int el);
    int find_parent(int l);
    std::tuple<int, int> find_range(int l);
    void set_cursor();
    void insert_entrylist(const vector<FileItem *> &, const int pos, vector<string> &ret);
    void insert_item(const int pos);
    void _toggle_select(const int pos);
    void collect_targets();
    void insert_rootcell(const int pos);
    void erase_entrylist(const int s, const int e);
    string makeline(const int pos);
    void entryInfoListRecursively(const FileItem &, vector<FileItem *> &fileitem_lst);

    void save_cursor();
    void shrinkRecursively(const string &path);
    void expandRecursively(const FileItem &, list<FileItem *> &fileitem_lst);
};

}  // namespace tree
#endif
