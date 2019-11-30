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
    void action(const string &action, const list<nvim::Object> &args, const unordered_map<string, nvim::Object> context);

    inline void buf_set_lines(int s, int e, bool strict, vector<string> &replacement)
    {
        api->nvim_buf_set_option(bufnr, "modifiable", true);
        api->nvim_buf_set_lines(bufnr, s, e, strict, replacement);
        api->nvim_buf_set_option(bufnr, "modifiable", false);
        // TODO: Fine-grained targets update
        // collect_targets();
    };

private:
    list<FileItem *> m_fileitem;
    unordered_map<int, list<Cell>> col_map;
    unordered_map<string, bool> expandStore;
    list<int> targets;
    void set_last(list<FileItem*> &fileitems);
    void hline(int sl, int el);
    int find_parent(int l);
    std::tuple<int, int> find_range(int l);
    void insert_entrylist(const list<FileItem*> &, const int pos, vector<string>& ret);
    void insert_item(const int pos);
    void collect_targets();
    void insert_rootcell(const int pos);
    void erase_entrylist(const int s, const int e);
    void makeline(const int pos, string &line);
    void entryInfoListRecursively(const FileItem &, list<FileItem*>& fileitem_lst);

    void shrinkRecursively(const string &path);
    void expandRecursively(const FileItem&, list<FileItem*> &fileitem_lst);
};


#endif
