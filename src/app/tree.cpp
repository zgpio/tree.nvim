#include <boost/filesystem.hpp>
#include <cinttypes>
#include <cwchar>
#include <codecvt>
#include <chrono>
#include "tree.h"
#include "strnatcmp.hpp"

#if defined(OS_WIN)
extern int mk_wcwidth(wchar_t ucs);
#endif
using namespace boost::filesystem;
using std::string;
using std::cout;
using std::endl;

namespace tree {
Tree::ClipboardMode Tree::paste_mode;
list<string> Tree::clipboard;
nvim::Nvim *Tree::api;

int wchar_width(wchar_t ucs)
{
#if defined(OS_WIN)
    return mk_wcwidth(ucs);
#else
    return wcwidth(ucs);
#endif
}
Tree::~Tree()
{
    erase_entrylist(0, m_fileitem.size());
}
Tree::Tree(int bufnr, int ns_id)
    : bufnr(bufnr), ns_id(ns_id)
{

    api->buf_set_option(bufnr, "ft", "tree");
    api->buf_set_option(bufnr, "modifiable", false);

    api->command("lua tree = require('tree')");
    // b->buf_attach(bufnr, false, {});
    api->execute_lua("tree.buf_attach(...)", {bufnr});
}

// Characters can be full-width or half-width (with invisible characters being 0-width).
// Ref:
//  https://bugreports.qt.io/browse/QTBUG-9956
//  https://www.php.net/manual/en/function.mb-strwidth.php
//  https://www.ibm.com/support/knowledgecenter/beta/fi/ssw_ibm_i_74/rtref/wcwidth.htm
//  https://embedeo.org/ws/doc/man_windows/
int countgrid(const std::wstring &s)
{
    int n = s.size();
    int ans = 0;
    for (int i = 0; i < n; i++) {
        const wchar_t wc = s.at(i);

        // NOTE: wcwidth need unicode
        if (wchar_width(wc)==2) {
            ans++;
        }
    }
    return n + ans;  // screen cells
}
// truncate to l screen cells
void truncate(const int l, Cell &cell)
{
    int old_bytesize = cell.text.size();
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    std::wstring str = converter.from_bytes(cell.text.c_str());
    int ls=0, rs=0;
    int i=0, j=str.size()-1;
    while (ls+rs < l-2) {
        if (ls<rs) {
            ls += wchar_width(str.at(i));
            i++;
        }
        else {
            rs += wchar_width(str.at(j));
            j--;
        }
    }
    str.erase(i, j-i+1);
    str.insert(i, L"…");
    // https://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string
    // use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    cell.text = converter.to_bytes(str);
    cell.byte_end -= old_bytesize - cell.text.size();
    cell.col_end -= cell.col_end - cell.col_start - (ls+rs+1);
}

// NOTE: depend on RVO
string Tree::makeline(const int pos)
{
    assert(0<=pos&&pos<col_map[FILENAME].size());
    string line;
    int start = 0;
    for (int col : cfg.columns) {
        const Cell & cell = col_map[col][pos];
        line.append(string(cell.col_start-start, ' '));
        line.append(cell.text);
        int len = cell.byte_end - cell.byte_start - cell.text.size();
        // string cell_str(cell.text);
        // if (col=="filename")
        //     len = cell.col_end-countgrid(cell_str)-cell.col_start;
        // else
        //     len = cell.col_end-cell_str.size()-cell.col_start;
        line.append(string(len, ' '));
        start = cell.col_end;
    }
    // INFO("pos:%d line:%s\n", pos, line.c_str());
    return line;
}
#define TEST
using namespace std::chrono;
void Tree::set_cursor()
{
    string k = (*m_fileitem[0]).p.string();
    auto got = cursorHistory.find(k);
    if (got != cursorHistory.end()) {
        api->async_win_set_cursor(0, {cursorHistory[k], 0});
    }
}
void Tree::changeRoot(const string &root)
{
#ifdef TEST
    auto start_change_root=system_clock::now();
#endif
    // TODO: cursor history
    path dir(root);
    if (!exists(dir)) {
        return;
    }
    const string & rootPath = dir.string();
    expandStore.insert({rootPath, true});

    auto i = find(cfg.columns.begin(), cfg.columns.end(), GIT);
    if (i != cfg.columns.end()) {
        FileItem::update_gmap(root);
    }
    targets.clear();
    erase_entrylist(0, m_fileitem.size());

    FileItem *fileitem = new FileItem(dir);
    fileitem->level = -1;
    fileitem->opened_tree = true;
    m_fileitem.push_back(fileitem);

    insert_rootcell(0);
    // FIXME: when icon not available
    // col_map[ICON][0].text = "";

    vector<string> ret;
    string line = makeline(0);
    ret.push_back(line);

    vector<FileItem*> child_fileitem;
    entryInfoListRecursively(*m_fileitem[0], child_fileitem);
    for (auto item:child_fileitem) {
        m_fileitem.push_back(item);
    }

    insert_entrylist(child_fileitem, 1, ret);

    buf_set_lines(0, -1, true, ret);

    hline(0, m_fileitem.size());
#ifdef TEST
    duration<double> time_change_root = system_clock::now()-start_change_root;
    printf("change_root Elapsed time:%.9f secs.\n", time_change_root.count());
#endif
}

/// Insert columns
void Tree::insert_item(const int pos)
{
    const FileItem &fileitem = *m_fileitem[pos];
    int start = 0;
    int byte_start = 0;
    const int kStop = cfg.filename_colstop;
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    for (const int col : cfg.columns) {
        Cell cell(cfg, fileitem, col);
        std::wstring ws = converter.from_bytes(cell.text.c_str());
        cell.byte_start = byte_start;
        cell.byte_end = byte_start+cell.text.size();
        cell.col_start = start;
        if (col==FILENAME) {
            cell.col_end = start + countgrid(ws);
            // NOTE: alignment
            int tmp = kStop - cell.col_end;
            if (tmp > 0) {
                cell.col_end+=tmp;
                cell.byte_end+=tmp;
            }
            else if (kStop>cell.col_start+5) {
                truncate(kStop - cell.col_start, cell);
                int tmp = kStop - cell.col_end;
                if (tmp > 0) {
                    cell.col_end+=tmp;
                    cell.byte_end+=tmp;
                }
            }
        }
        else {
            cell.col_end = start + ws.size();
        }

        int sep = (col==INDENT?0:1);
        start = cell.col_end + sep;
        byte_start = cell.byte_end + sep;

        col_map[col].emplace(col_map[col].begin()+pos, std::move(cell));
    }
}

void Tree::insert_rootcell(const int pos)
{
    const FileItem &fileitem = *m_fileitem[pos];
    int start = 0;
    int byte_start = 0;
    const int kStop = cfg.filename_colstop;
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    for (int col : cfg.columns) {
        Cell cell(cfg, fileitem, col);
        cell.col_start = start;
        cell.byte_start = byte_start;
        if (col==FILENAME) {
            string text(fileitem.p.string());
            if (text.back() != '/' && is_directory(fileitem.p)) {
                text.append("/");
            }
            text.insert(0, cfg.root_marker.c_str());
            cell.text = text;
        }
        std::wstring ws = converter.from_bytes(cell.text.c_str());
        cell.byte_end = byte_start+cell.text.size();

        if (col==FILENAME) {
            cell.col_end = start + countgrid(ws);
            // NOTE: alignment
            int tmp = kStop - cell.col_end;
            if (tmp >0) {
                cell.col_end+=tmp;
                cell.byte_end+=tmp;
            }
        }
        else {
            cell.col_end = start + ws.size();
        }

        int sep = (col==INDENT?0:1);
        start = cell.col_end + sep;
        byte_start = cell.byte_end + sep;

        col_map[col].emplace(col_map[col].begin()+pos, std::move(cell));
    }
}
/// pos is 0-based row number.
void Tree::insert_entrylist(const vector<FileItem*>& fil, const int pos, vector<string>& ret)
{
    int file_count = fil.size();
    for (int i = 0; i < file_count; ++i) {
        insert_item(pos+i);

        string line = makeline(pos+i);
        ret.push_back(std::move(line));
    }
}
// TODO: Find the parent through FileItem.parent
/// l is 0-based row number.
/// NOTE: root.level=-1
int Tree::find_parent(int l)
{
    int level = m_fileitem[l]->level;
    for (int i=l-1;i>=0;--i)
    {
        const FileItem &fn = *m_fileitem[i];
        if (fn.level == level-1)
            return i;
    }
    return -1;
}

/// [l, l] <=> no sub files; l is parent row number(0-based).
std::tuple<int, int> Tree::find_range(int l)
{
    int s=l, i;
    int level = m_fileitem[l]->level;
    for (i=l+1;i<m_fileitem.size();++i)
    {
        int l = m_fileitem[i]->level;
        if (level >= l)
            break;
    }
    return std::make_tuple(s, i-1);
}

/// 0-based [sl, el).
void Tree::hline(int sl, int el)
{
    int bufnr = this->bufnr;
    api->async_buf_clear_namespace(bufnr, ns_id, sl, el);
    for (int i = sl;i<el;++i)
    {
        const FileItem &fileitem = *m_fileitem[i];
        char name[32];

        for (const int col : cfg.columns) {
            const Cell & cell = col_map[col][i];

            if(col==FILENAME) {
                sprintf(name, "tree_%u_%u", col, is_directory(fileitem.p));
                api->async_buf_add_highlight(bufnr, ns_id, name, i, cell.byte_start, cell.byte_start+cell.text.size());
            } else if(col==ICON || col==GIT || col==MARK) {
                // :hi tree_<tab>
                sprintf(name, "tree_%u_%u", col, cell.color);
                // sprintf(name, "tree_%s", cell.text.data());
                // auto req_hl = api->buf_add_highlight(bufnr, 0, "String", 0, 0, 3);
                // call buf_add_highlight(0, -1, "Identifier", 0, 5, -1)
                api->async_buf_add_highlight(bufnr, ns_id, name, i, cell.byte_start, cell.byte_start+cell.text.size());
            } else if (col==SIZE || col==TIME || col==INDENT){
                sprintf(name, "tree_%u", col);
                api->async_buf_add_highlight(bufnr, ns_id, name, i, cell.byte_start, cell.byte_start+cell.text.size());
            }
        }
    }
}

/// 0-based [sl, el).
void Tree::redraw_line(int sl, int el)
{
    INFO("(1-based): [%d, %d]\n", sl+1, el);

    vector<string> ret;
    const int kStop = cfg.filename_colstop;
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    for (int i = sl; i < el; ++i) {
        FileItem & fileitem = *m_fileitem[i];

        int start = 0;
        int byte_start = 0;
        for (const int col : cfg.columns) {
            Cell& cell = col_map[col][i];
            if (col==MARK){
            }else if(col==INDENT){
            }else if(col==GIT){
                // TODO: Git::update_gmap(fn);
                cell.update_git(fileitem);
            }else if(col==ICON){
                cell.update_icon(fileitem);
            } else if(col==FILENAME){
            }else if(col==SIZE){
                cell.update_size(fileitem);
            }
            std::wstring cell_str = converter.from_bytes(cell.text.c_str());
            cell.col_start =start;
            cell.col_end = start + countgrid(cell_str);
            cell.byte_start = byte_start;
            cell.byte_end = byte_start+cell.text.size();

            if (col==FILENAME)
            {
                int tmp = kStop - cell.col_end;
                if (tmp >0)
                {
                    cell.col_end+=tmp;
                    cell.byte_end+=tmp;
                }
            }
            int sep = (col==INDENT?0:1);
            start = cell.col_end + sep;
            byte_start = cell.byte_end + sep;
        }

        string line = makeline(i);
        ret.push_back(std::move(line));
    }
    buf_set_lines(sl, el, true, std::move(ret));
    hline(sl, el);
}
/// redraw parent and children
/// assert l is 0-based parent index
void Tree::redraw_recursively(int l)
{
    assert(0 <= l && l < m_fileitem.size());
    FileItem &cur = *m_fileitem[l];

    std::tuple<int, int> se = find_range(l);
    int s = std::get<0>(se) + 1;
    int e = std::get<1>(se) + 1;
    INFO("redraw range(1-based): [%d, %d]\n", s+1, e);

    erase_entrylist(s, e);

    vector<FileItem*> child_fileitem;
    // const string &p = cur.p.string();
    entryInfoListRecursively(cur, child_fileitem);
    int file_count = child_fileitem.size();
    for (int i = 0; i < file_count; ++i)
        m_fileitem.insert(m_fileitem.begin() + l + 1 + i, child_fileitem[i]);

    if (file_count <= 0) {
        return;
    }

    vector<string> ret;
    insert_entrylist(child_fileitem, l + 1, ret);
    buf_set_lines(s, e, true, ret);
    hline(l + 1, l + 1 + ret.size());

    return;
}
/// erase [s, e)
void Tree::erase_entrylist(const int s, const int e)
{
    for (const int col : cfg.columns) {
        vector<Cell> & cell = col_map[col];
        cell.erase(cell.begin()+s, cell.begin()+e);
    }
    for (int i = s; i < e; ++i) {
        delete m_fileitem[i];
    }
    m_fileitem.erase(m_fileitem.begin()+s, m_fileitem.begin()+e);
}

// get entryInfoList recursively
void Tree::entryInfoListRecursively(const FileItem& item,
                                   vector<FileItem*> &fileitem_lst)
{
    path dir(item.p);
    const int level = item.level+1;
    typedef vector<path> vec;             // store paths,
    vec v;                                // so we can sort them later
    try {
        if (cfg.show_ignored_files)
            copy(directory_iterator(item.p), directory_iterator(), back_inserter(v));
        else
            copy_if(directory_iterator(item.p), directory_iterator(), back_inserter(v),
                [](auto x){return (x.path().filename().string().front() != '.');});
    } catch(std::exception& e) {
        INFO("-------> %s\n", e.what());
        return;
    }
    sort(v.begin(), v.end(), [](path x, path y) {
        if (is_directory(x) == is_directory(y))
            return compareNat(x.string(), y.string());
        else
            return is_directory(x) > is_directory(y);
    });

    for (auto &x : v) {
      try {
        FileItem *fileitem = new FileItem(x);
        fileitem->level = level;
        fileitem->parent = &item;
        if (&x == &(*(v.end()-1))) {
            fileitem->last = true;
        }

        auto search = expandStore.find(fileitem->p.string());
        if (search != expandStore.end() && search->second) {
            fileitem->opened_tree = true;
            fileitem_lst.push_back(fileitem);
            entryInfoListRecursively(*fileitem, fileitem_lst);
        }
        else
            fileitem_lst.push_back(fileitem);
      } catch(std::exception& e) {
          continue;
      }
    }
}

// shrink recursively
void Tree::shrinkRecursively(const string &p)
{
    path dir(p);
    // set_dir(dir);
    typedef vector<path> vec;             // store paths,
    vec v;                                // so we can sort them later
    try {
        copy(directory_iterator(p), directory_iterator(), back_inserter(v));
    } catch(std::exception& e) {
        INFO("-------> %s\n", e.what());
        return;
    }

    for (auto x : v) {
        string p = x.string();

        auto got = expandStore.find(p);
        if (got != expandStore.end() && expandStore[p]) {
            expandStore[p] = false;
            shrinkRecursively(p);
        }
    }
    return;
}

void Tree::expandRecursively(const FileItem &item, vector<FileItem*> &fileitems)
{
    const int level = item.level+1;
    typedef vector<directory_entry> vec;             // store paths,
    vec v;                                // so we can sort them later
    try {
        copy(directory_iterator(item.p), directory_iterator(), back_inserter(v));
    } catch(std::exception& e) {
        INFO("-------> %s\n", e.what());
        return;
    }
    sort(v.begin(), v.end(), [](path x, path y) {
        if (is_directory(x) == is_directory(y))
            return compareNat(x.string(), y.string());
        else
            return is_directory(x) > is_directory(y);
    });

    for (directory_entry &x : v) {
      try {
        FileItem *fileitem = new FileItem(x);
        fileitem->level = level;
        fileitem->parent = &item;
        if (&x == &(*(v.end()-1))) {
            fileitem->last = true;
        }

        if (is_directory(x)) {
            string p = x.path().string();
            expandStore[p] = true;
            fileitem->opened_tree = true;
            fileitems.push_back(fileitem);
            expandRecursively(*fileitem, fileitems);
        }
        else
            fileitems.push_back(fileitem);
      } catch(std::exception& e) {
          continue;
      }
    }
}

void Tree::handleRename(string &input)
{
    INFO("\n");

    Cell & cur = col_map[FILENAME][ctx.cursor-1];
    FileItem & item = *m_fileitem[ctx.cursor-1];
    if (!is_directory(item.p) && input.back() == '/')
        input.pop_back();
    string fn = item.p.string();
    boost::filesystem::rename(item.p, input);
    api->async_execute_lua("tree.print_message(...)", {"Rename Success"});
    string text(item.p.filename().string());
    if (is_directory(item.p))
        text.append("/");
    cur.text = text;

    // NOTE: gmap may update
    // FileItem::update_gmap(item.fi.absolutePath());
    // redraw_line(ctx.cursor-1, ctx.cursor);
    // TODO: Fine-grained redraw
    FileItem &root = *m_fileitem[0];
    changeRoot(root.p.string());

    // api->execute_lua("tree.print_message(...)", {"Rename failed"});

}
void Tree::handleNewFile(const string &input)
{
    if (input=="") {
        api->async_execute_lua("tree.print_message(...)", {"Canceled"});
        return;
    }
    INFO("input: %s\n", input.c_str());

    // Cell & cur = col_map["filename"][ctx.cursor-1];
    FileItem & item = *m_fileitem[ctx.cursor-1];

    path dest = item.opened_tree ? item.p : item.p.parent_path();

    dest /= input;
    INFO("dest: %s\n", dest.string().c_str());
    // QFileInfo fi(dest.filePath(input));
    // NOTE: failed when same name file exists
    // TODO: No case sensitive on macos 10.14.5; Works on linux.
    if (boost::filesystem::exists(dest)) {
        api->async_execute_lua("tree.print_message(...)", {"File already exists!"});
        return;
    }
    else if(input.back() == '/'){
        if(!create_directory(dest))
            api->async_execute_lua("tree.print_message(...)", {"Failed to create dir!"});
    } else {
        boost::filesystem::ofstream(dest.string());
    }

    if (item.opened_tree) {
        redraw_recursively(ctx.cursor-1);
    } else{
        // TODO: location: find_child
        int pidx = find_parent(ctx.cursor-1);
        // NOTE: root.level=-1
        // TODO: consider named redraw_family
        redraw_recursively(pidx);
    }
    // TODO: Find in subdirectories is faster
    for (int i = 0; i < m_fileitem.size(); i++) {
        if (m_fileitem[i]->p == dest) {
            api->async_win_set_cursor(0, {i + 1, 0});
            break;
        }
    }
}

/// 收集无序targets
/// 视图变化之后 targets 要更新
void Tree::collect_targets()
{
    targets.clear();
    for (int i = 0; i < m_fileitem.size(); ++i) {
        const FileItem &item = *m_fileitem[i];
        if (item.selected) {
            targets.push_back(i);
        }
    }
}
void Tree::save_cursor()
{
    FileItem &item = *m_fileitem[0];
    cursorHistory[item.p.string()] = ctx.cursor;
    INFO("cursorHistory: %s -> %d\n", item.p.string().c_str(), ctx.cursor);
}
void Tree::paste(const int ln, const string &src, const string &dest)
{
    if (is_directory(src)) {
        if (paste_mode == COPY) {
            copy(src, dest);
            api->async_execute_lua("tree.print_message(...)", {"Copyed"});
            INFO("Copy Paste dir\n");
            int pidx = find_parent(ln);
            redraw_recursively(pidx);
        }
        else if (paste_mode == MOVE){
            boost::filesystem::rename(src, dest);
            INFO("Move Paste dir\n");
            FileItem &root = *m_fileitem[0];
            changeRoot(root.p.string());
        }
    }
    else {
        if (paste_mode == COPY) {
            copy(src, dest);
            api->async_execute_lua("tree.print_message(...)", {"Copyed"});
            INFO("Copy Paste\n");
            int pidx = find_parent(ln);
            redraw_recursively(pidx);
        }
        else if (paste_mode == MOVE){
            boost::filesystem::rename(src, dest);
            INFO("Move Paste\n");
            FileItem &root = *m_fileitem[0];
            changeRoot(root.p.string());
        }
    }
    return;
}
typedef void (Tree::*Action)(const nvim::Array& args);
std::unordered_map<string, Action> action_map {
    {"cd"                   , &Tree::cd},
    {"goto"                 , &Tree::goto_},
    {"open_or_close_tree"   , &Tree::open_tree},
    {"open"                 , &Tree::open},
    {"copy"                 , &Tree::copy_},
    {"move"                 , &Tree::move},
    {"paste"                , &Tree::pre_paste},
    {"remove"               , &Tree::pre_remove},
    {"yank_path"            , &Tree::yank_path},
    {"toggle_select"        , &Tree::toggle_select},
    {"toggle_select_all"    , &Tree::toggle_select_all},
    {"print"                , &Tree::print},
    {"debug"                , &Tree::debug},
    {"toggle_ignored_files" , &Tree::toggle_ignored_files},
    {"redraw"               , &Tree::redraw},
    {"new_file"             , &Tree::new_file},
    {"execute_system"       , &Tree::execute_system},
    {"rename"               , &Tree::rename},
    {"drop"                 , &Tree::drop},
    {"call"                 , &Tree::call},
    {"view"                 , &Tree::view},
    {"open_tree_recursive"  , &Tree::open_or_close_tree_recursively},
};
void Tree::action(const string &action, const nvim::Array &args,
                  const Map &context)
{
    INFO("action: %s\n", action.c_str());

    this->ctx = context;
    INFO("cursor position(1-based): %d\n", ctx.cursor);

    auto search = action_map.find(action);
    if (search != action_map.end()) {
        (this->*action_map[action])(args);
    }
    else {
        api->call_function("tree#util#print_message", {"Unknown Action: " + action});
    }
}

/// l is 0-based row number.
void Tree::open_tree(const nvim::Array &args)
{
    const int l = ctx.cursor - 1;
    INFO("\n");
    assert(0 <= l && l < m_fileitem.size());
    // if (l == 0) return;
    vector<string> ret;
    FileItem &cur = *m_fileitem[l];

    if (is_directory(cur.p) && !cur.opened_tree) {

        cur.opened_tree = true;
        const string & rootPath = cur.p.string();
        expandStore[rootPath] = true;
        redraw_line(l, l + 1);
        vector<FileItem*> child_fileitem;
        entryInfoListRecursively(cur, child_fileitem);
        int file_count = child_fileitem.size();
        for (int i=0;i<file_count;++i) {
            m_fileitem.insert(m_fileitem.begin()+l+1+i, child_fileitem[i]);
        }

        if (file_count <= 0) {
            return;
        }

        insert_entrylist(child_fileitem, l + 1, ret);

        buf_set_lines(l+1, l+1, true, ret);
        hline(l + 1, l + 1 + ret.size());
    }
    else if (cur.opened_tree) {
        const string & p = cur.p.string();
        auto search = expandStore.find(p);
        if (search != expandStore.end() && expandStore[p]) {
            expandStore[p] = false;
        }
        std::tuple<int, int> se = find_range(l);
        int s = std::get<0>(se) + 1;
        int e = std::get<1>(se) + 1;
        printf("\tclose range(1-based): [%d, %d]\n", s+1, e);
        buf_set_lines(s, e, true, {});

        erase_entrylist(s, e);
        cur.opened_tree = false;
        redraw_line(l, l + 1);
    }
    else if (find_parent(l) >= 0) {
        int parent = find_parent(l);
        std::tuple<int, int> se = find_range(parent);
        int s = std::get<0>(se) + 1;
        int e = std::get<1>(se) + 1;

        printf("\tclose range(1-based): [%d, %d]\n", s+1, e);

        buf_set_lines(s, e, true, {});
        // ref to https://github.com/equalsraf/neovim-qt/issues/596
        api->async_win_set_cursor(0, {s, 0});
        erase_entrylist(s, e);

        FileItem &father = *m_fileitem[parent];
        father.opened_tree = false;
        const string & p = father.p.string();
        auto got = expandStore.find(p);
        if (got != expandStore.end() && expandStore[p]) {
            expandStore[p] = false;
        }
        redraw_line(parent, parent + 1);
    }
    return;
}

Map Tree::get_candidate(const int pos)
{
    // 'word': 'column.cpp',
    FileItem & item = *m_fileitem[pos];
    return {
        {"is_directory", is_directory(item.p)},
        {"action__path", item.p.string()},
        {"level", item.level},
        {"is_opened_tree", item.opened_tree},
        {"is_selected", item.selected}
    };
}
void Tree::open(const nvim::Array &args)
{
    save_cursor();
    const int l = ctx.cursor - 1;
    const path &p = m_fileitem[l]->p;
    if (is_directory(p)) {
        changeRoot(p.string());
    }
    else if (args.size()>0 && args[0].as_string()=="vsplit") {
        INFO("vsplit: %s\n", p.string().c_str());
        api->async_call_function("tree#util#execute_path", {"rightbelow vsplit", p.string()});
    }
    else {
        api->async_call_function("tree#util#execute_path", {"drop", p.string()});
    }
}

void Tree::rename(const nvim::Array &args)
{
    FileItem &cur = *m_fileitem[ctx.cursor - 1];
    string info = cur.p.string();
    nvim::Dictionary cfg{
        {"prompt", "Rename: " + info + " -> "},
        {"text", info},
        {"completion", "file"},
        {"handle", "rename"},
        {"bufnr", bufnr}
    };
    api->async_execute_lua("tree.rename(...)", {cfg});
}

void Tree::drop(const nvim::Array &args)
{
    FileItem &cur = *m_fileitem[ctx.cursor - 1];
    const auto p = cur.p;
    if (is_directory(cur.p))
        changeRoot(p.string());
    else {
        api->async_execute_lua("tree.drop(...)", {args, p.string()});
    }
}
// TODO Handle calls without viewing permission
void Tree::cd(const nvim::Array &args)
{
    save_cursor();
    if (args.size()>0) {
        string dir = args.at(0).as_string();

        if (dir=="..") {
            path & curdir = m_fileitem[0]->p;
            INFO("cd %s\n", curdir.parent_path().string().c_str());
            changeRoot(curdir.parent_path().string());
        }
        else if (dir == ".") {
            FileItem &cur = *m_fileitem[ctx.cursor - 1];
            string dir = is_directory(cur.p) ? cur.p.string() : cur.p.parent_path().string();
            string cmd = "cd " + dir;
            api->async_execute_lua("tree.print_message(...)", {cmd});
            api->async_command(cmd);
        }
        else {
            changeRoot(dir);
        }
        set_cursor();
    }
    else {
        // changeRoot(QDir::home().absolutePath());
    }
}
void Tree::call(const nvim::Array &args)
{
    string func = args.at(0).as_string();
    FileItem &cur = *m_fileitem[ctx.cursor - 1];
    Map ctx = {
        {"targets", cur.p.string()}
    };
    api->async_call_function(func, {ctx});
}

void Tree::print(const nvim::Array &args)
{
    FileItem &cur = *m_fileitem[ctx.cursor - 1];
    string msg = cur.p.string();
    string msg2 = "last=" + string(cur.last ? "true" : "false");
    string msg3 = "level=" + std::to_string(cur.level);
    api->async_execute_lua("tree.print_message(...)", {msg+" "+msg2+" "+msg3});
}
void Tree::pre_paste(const nvim::Array &args)
{
    if (clipboard.size() <= 0) {
        api->async_execute_lua("tree.print_message(...)", {"Nothing in clipboard"});
        return;
    }
    for (const string &f : clipboard) {
        // TODO Remove non-existent source directories from the clipboard
        if (!exists(f))
            continue;
        FileItem &cur = *m_fileitem[ctx.cursor - 1];
        string fname = path(f).filename().string();
        path curdir = cur.p.parent_path();
        if (cur.opened_tree) curdir = cur.p.string();
        string destfile = (curdir/=fname).string();
        INFO("destfile: %s\n", destfile.c_str());
        INFO("fname: %s\n", fname.c_str());
        if (exists(destfile)) {
            // api->async_execute_lua("tree.print_message(...)", {"Destination file exists"});

            Map dest {
                {"mtime", last_write_time(destfile)},
                {"path", destfile},
                {"size", file_size(destfile)},
            };

            Map src {
                {"mtime", last_write_time(f)},
                {"path", f},
                {"size", file_size(f)},
            };
            api->async_execute_lua("tree.pre_paste(...)",
                {nvim::Array{bufnr, ctx.cursor - 1}, dest, src});
        }
        else {
            paste(ctx.cursor-1, f, destfile);
        }
    }
}

void Tree::debug(const nvim::Array &args)
{
    for (auto i : cfg.columns) {
        cout << i << ":";
    }
    cout << endl;
    for (auto i : expandStore) {
        cout << i.first << ":" << i.second << endl;
    }
    for (auto i : FileItem::git_map) {
        cout << i.first << ":" << i.second << endl;
    }
    FileItem &cur = *m_fileitem[ctx.cursor - 1];
}
void Tree::yank_path(const nvim::Array &args)
{
    vector<string> yank;
    for (const int &pos : targets) {
        yank.push_back(m_fileitem[pos]->p.string());
    }
    if (yank.size()==0) {
        FileItem &cur = *m_fileitem[ctx.cursor - 1];
        yank.push_back(cur.p.string());
    }
    string reg;
    for (auto i : yank) {
        reg += i;
        reg += '\n';
    }
    api->async_call_function("setreg", {"\"", reg});

    reg.insert(0, "yank_path\n");
    api->async_execute_lua("tree.print_message(...)", {reg});
}
void Tree::pre_remove(const nvim::Array &args)
{
    int cnt = targets.size();
    Map farg;
    farg.insert({"cnt", cnt==0 ? 1:cnt});
    api->async_execute_lua("tree.pre_remove(...)", {bufnr, farg});
}
void Tree::remove()
{
    // TODO: cursor position after remove
    save_cursor();
    vector<string> rmfiles;
    for (const int &pos : targets) {
        rmfiles.push_back(m_fileitem[pos]->p.string());
    }
    if (rmfiles.size()==0) {
        FileItem &cur = *m_fileitem[ctx.cursor - 1];
        rmfiles.push_back(cur.p.string());
    }
    for (const string &f : rmfiles) {
        cout << f << endl;
        if (is_directory(f))
            boost::filesystem::remove_all(f);
        else
            boost::filesystem::remove(f);
    }
    FileItem &root = *m_fileitem[0];
    changeRoot(root.p.string());
    set_cursor();
}
void Tree::redraw(const nvim::Array &args)
{
    FileItem &root = *m_fileitem[0];
    changeRoot(root.p.string());
}
void Tree::new_file(const nvim::Array &args)
{
    FileItem &cur = *m_fileitem[ctx.cursor - 1];
    string info;
    if (cur.opened_tree) {
        info = cur.p.string();
    } else {
        int p = find_parent(ctx.cursor-1);
        info = m_fileitem[p]->p.string();
    }
    nvim::Dictionary cfg{
        {"prompt", "New File: " + info + "/"},
        {"text", ""},
        {"completion", "file"},
        {"handle", "new_file"},
        {"bufnr", bufnr}
    };
    api->async_execute_lua("tree.new_file(...)", {cfg});
}
void Tree::_toggle_select(const int pos)
{
    // TODO: mark may not available
    Cell &cur = col_map[MARK][pos];
    FileItem& item = *m_fileitem[pos];

    item.selected = !item.selected;
    if (item.selected) {
        cur.text = mark_indicators["selected_icon"];
        cur.color = BLUE;
        targets.push_back(pos);
    }
    else {
        cur.text=" ";
        cur.color = WHITE;
        targets.remove(pos);
    }

    redraw_line(pos, pos+1);
}
void Tree::toggle_select(const nvim::Array &args)
{
    const int pos = ctx.cursor - 1;
    _toggle_select(pos);
}
void Tree::toggle_select_all(const nvim::Array &args)
{
    for (int i=1;i<m_fileitem.size();++i) {
        _toggle_select(i);
    }
}
void Tree::_copy_or_move(const nvim::Array &args)
{
    Tree::clipboard.clear();

    for (const int &pos : targets) {
        const FileItem *p = m_fileitem[pos]->parent;
        // NOTE: root item or parent selected
        if (p==nullptr || !p->selected)
            Tree::clipboard.push_back(m_fileitem[pos]->p.string()) ;
    }
    if (clipboard.size()==0) {
        FileItem &cur = *m_fileitem[ctx.cursor - 1];
        Tree::clipboard.push_back(cur.p.string());
    }
    for (auto i:clipboard)
        cout << i << endl;
}

void Tree::copy_(const nvim::Array &args)
{
    paste_mode = COPY;
    _copy_or_move(args);
    api->async_execute_lua("tree.print_message(...)", {"Copy to clipboard"});
}
void Tree::move(const nvim::Array &args)
{
    paste_mode = MOVE;
    _copy_or_move(args);
    api->async_execute_lua("tree.print_message(...)", {"Move to clipboard"});
}
void Tree::open_or_close_tree_recursively(const nvim::Array &args)
{
    const int l = ctx.cursor - 1;
    INFO("\n");
    assert(0 <= l && l < m_fileitem.size());
    if (l == 0) return;
    vector<string> ret;
    FileItem &cur = *m_fileitem[l];

    if (is_directory(cur.p) && !cur.opened_tree) {
        cur.opened_tree = true;
        const string &rootPath = cur.p.string();
        expandStore.insert({rootPath, true});
        redraw_line(l, l + 1);
        vector<FileItem*> child_fileitem;
        expandRecursively(cur, child_fileitem);
        int file_count = child_fileitem.size();
        for (int i = 0; i < file_count; ++i)
            m_fileitem.insert(m_fileitem.begin() + l + 1 + i, child_fileitem[i]);

        if (file_count <= 0) {
            return;
        }

        insert_entrylist(child_fileitem, l + 1, ret);
        buf_set_lines(l+1, l+1, true, ret);
        hline(l + 1, l + 1 + ret.size());
        return;
    }
    else if (cur.opened_tree) {
        const string &p = cur.p.string();
        auto got = expandStore.find(p);
        if (got != expandStore.end() && expandStore[p]) {
            expandStore[p] = false;
        }
        std::tuple<int, int> se = find_range(l);
        int s = std::get<0>(se) + 1;
        int e = std::get<1>(se) + 1;
        printf("\tclose range(1-based): [%d, %d]\n", s+1, e);
        buf_set_lines(s, e, true, {});
        shrinkRecursively(p);
        erase_entrylist(s, e);
        cur.opened_tree = false;
        redraw_line(l, l + 1);
        return;
    }
    else if (find_parent(l) >= 0) {
        int parent = find_parent(l);
        std::tuple<int, int> se = find_range(parent);
        int s = std::get<0>(se) + 1;
        int e = std::get<1>(se) + 1;

        printf("\tclose range(1-based): [%d, %d]\n", s+1, e);

        buf_set_lines(s, e, true, {});
        // ref to https://github.com/equalsraf/neovim-qt/issues/596
        api->async_win_set_cursor(0, {s, 0});

        FileItem &father = *m_fileitem[parent];
        father.opened_tree = false;
        const string &p = father.p.string();
        auto got = expandStore.find(p);
        if (got != expandStore.end() && expandStore[p]) {
            expandStore[p] = false;
        }
        shrinkRecursively(p);
        erase_entrylist(s, e);
        redraw_line(parent, parent + 1);
        return;
    }

    return;
}
void Tree::goto_(const nvim::Array &args)
{
    const int l = ctx.cursor - 1;

    if (args.size()>0) {
        string dest = args.at(0).as_string();

        if (dest=="parent") {
            int p = find_parent(l);
            api->async_win_set_cursor(0, {p+1, 0});
        }
    }
    else {
    }
}
void Tree::execute_system(const nvim::Array &args)
{
    FileItem &cur = *m_fileitem[ctx.cursor - 1];
    string info = cur.p.string();
    api->async_execute_lua("tree.open(...)", {info});
}
void Tree::toggle_ignored_files(const nvim::Array &args)
{
    cfg.show_ignored_files = !cfg.show_ignored_files;
    FileItem &root = *m_fileitem[0];
    changeRoot(root.p.string());
}
// TODO Custom display content
void Tree::view(const nvim::Array &args)
{
    FileItem &cur = *m_fileitem[ctx.cursor - 1];
    Cell time_cell(cfg, cur, TIME);
    string size = Cell(cfg, cur, SIZE).text;
    size.erase(0, size.find_first_not_of(" "));
    size.erase(size.find_last_not_of(" ") + 1);
    nvim::Dictionary info{
        {"filename", cur.filename},
        {"date", time_cell.text},
        {"size", size},
    };
    api->command("lua require('tree/float')");
    api->execute_lua("Tree_display(...)", {info});
}
} // namespace tree
