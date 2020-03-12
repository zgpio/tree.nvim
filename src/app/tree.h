#ifndef NVIM_CPP_TREE
#define NVIM_CPP_TREE

#include <list>
#include <tuple>
#include <unordered_map>
#include <boost/filesystem.hpp>
#include "column.h"
#include "nvim.hpp"

using std::list;
using std::string;
using std::vector;
using std::unordered_map;
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

    void changeRoot(const string &path);
    void action(const string &action, const nvim::Array &args, const Map &context);

    void redraw(const nvim::Array &args);
    void yank_path(const nvim::Array &args);
    void print(const nvim::Array &args);
    void debug(const nvim::Array &args);
    void open_tree(const nvim::Array &args);
    void open(const nvim::Array &args);
    void drop(const nvim::Array &args);
    void call(const nvim::Array &args);
    void rename(const nvim::Array &args);
    void vim_input(string prompt, string text, string completion, string handle);
    void expandRecursively(const FileItem &item, vector<FileItem*> &fileitem_lst);
    void cd(const nvim::Array &args);

    inline void buf_set_lines(int s, int e, bool strict, const vector<string> &replacement)
    {
        api->async_nvim_buf_set_option(bufnr, "modifiable", true);
        api->async_nvim_buf_set_lines(bufnr, s, e, strict, replacement);
        api->async_nvim_buf_set_option(bufnr, "modifiable", false);
        // TODO: Fine-grained targets update
        // collect_targets();
    };

private:
    vector<FileItem *> m_fileitem;
    unordered_map<int, vector<Cell>> col_map;
    unordered_map<string, bool> expandStore;
    list<int> targets;
    void set_last(vector<FileItem*> &fileitems);
    void hline(int sl, int el);
    int find_parent(int l);
    std::tuple<int, int> find_range(int l);
    void insert_entrylist(const vector<FileItem*> &, const int pos, vector<string>& ret);
    void insert_item(const int pos);
    void collect_targets();
    void insert_rootcell(const int pos);
    void erase_entrylist(const int s, const int e);
    void makeline(const int pos, string &line);
    void entryInfoListRecursively(const FileItem &, vector<FileItem*>& fileitem_lst);

    void shrinkRecursively(const string &path);
    void expandRecursively(const FileItem&, list<FileItem*> &fileitem_lst);
};


#endif
