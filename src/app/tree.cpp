#include <boost/filesystem.hpp>
#include <cinttypes>
#include <cwchar>
#include <codecvt>
#include "tree.h"

#if defined(Q_OS_WIN)
extern int mk_wcwidth(wchar_t ucs);
#endif
using namespace boost::filesystem;
using std::string;
using std::cout;
using std::endl;

Tree::ClipboardMode Tree::paste_mode;
list<string> Tree::clipboard;
nvim::Nvim *Tree::api;
const int kStop = 90;

Tree::~Tree()
{
    erase_entrylist(0, m_fileitem.size());
}
Tree::Tree(int bufnr, int ns_id)
    : bufnr(bufnr), icon_ns_id(ns_id)
{

    api->nvim_buf_set_option(bufnr, "ft", "tree");
    api->nvim_buf_set_option(bufnr, "modifiable", false);

    api->nvim_command("lua require('tree')");
    // b->nvim_buf_attach(bufnr, false, {});
    api->nvim_execute_lua("buf_attach(...)", {bufnr});
}

// Checked
int countgrid(const std::wstring &s)
{
    int n = s.size();
    int ans = 0;
    for (int i = 0; i < n; i++) {
        const wchar_t wc = s.at(i);

        // NOTE: wcwidth need unicode
#if defined(Q_OS_WIN)
        if (mk_wcwidth(wc)==2) {
            ans++;
        }
#else
        if (wcwidth(wc)==2) {
            ans++;
        }
#endif
    }
    return n + ans;  // screen cells
}
// Checked
void truncate(const int l, Cell &cell)
{
    int old_bytesize = cell.text.size();
    int n = cell.col_end - cell.col_start - l;
    int total = 0; // total visual width
    int pos = 1;
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    std::wstring str = converter.from_bytes(cell.text.c_str());
    while (total <= n) {
#if defined(Q_OS_WIN)
        total += mk_wcwidth(str.at(pos));
#else
        total += wcwidth(str.at(pos));
#endif
        str.erase(pos, 1);
    }
    str.insert(pos, L"…");
    // https://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string
    // use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    cell.text = converter.to_bytes(str);
    cell.byte_end -= old_bytesize - cell.text.size();
    cell.col_end -= total - 1;
}

void Tree::makeline(const int pos, string &line)
{
    assert(0<=pos&&pos<col_map[FILENAME].size());
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
        string spc_after(len, ' ');
        line.append(std::move(spc_after));
        start = cell.col_end;
    }
    // cout<<"mkline"<<pos<<":"<<line;
}
void Tree::changeRoot(const string &root)
{
    // TODO: cursor history
    path dir(root);
    if (!exists(dir)) {
        return;
    }
    const string & rootPath = dir.string();
    expandStore.insert({rootPath, true});

    FileItem::update_gmap(root);
    targets.clear();
    erase_entrylist(0, m_fileitem.size());

    FileItem *fileitem = new FileItem;
    fileitem->level = -1;
    fileitem->opened_tree = true;
    fileitem->fi = status(dir);
    fileitem->p = dir;
    fileitem->filename = rootPath;
    m_fileitem.push_back(fileitem);

    insert_rootcell(0);
    // FIXME: when icon not available
    // col_map["icon"][0].text = "";

    vector<string> ret;
    string line;
    makeline(0, line);
    ret.push_back(line);

    vector<FileItem*> child_fileitem;

    entryInfoListRecursively(*m_fileitem[0], child_fileitem);
    set_last(child_fileitem);
    for (auto item:child_fileitem) {
        m_fileitem.push_back(item);
    }

    insert_entrylist(child_fileitem, 1, ret);

    buf_set_lines(0, -1, true, ret);

    hline(0, m_fileitem.size());
}

/// Insert columns
void Tree::insert_item(const int pos)
{
    const FileItem &fileitem = *m_fileitem[pos];
    int start = 0;
    int byte_start = 0;
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    for (const int col : cfg.columns) {
        Cell cell(cfg, fileitem, col);
        std::wstring ws = converter.from_bytes(cell.text.c_str());
        cell.byte_start = byte_start;
        cell.byte_end = byte_start+cell.text.size();
        cell.col_start = start;
        if (col==FILENAME) {
            cell.col_end = start + countgrid(ws);
        }
        else {
            cell.col_end = start + ws.size();
        }

        // NOTE: alignment
        if (col==FILENAME) {
            int tmp = kStop - cell.col_end;
            // TODO:此处都设置成统一列，在makeline时进行截断
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

        int sep = (col==INDENT?0:1);
        start = cell.col_end + sep;
        byte_start = cell.byte_end + sep;

        auto search = col_map.find(col);
        if (search != col_map.end()) {
            col_map[col].emplace(search->second.begin()+pos, std::move(cell));
        } else {
            col_map.insert({col, vector<Cell>()});
        }
    }
}

void Tree::insert_rootcell(const int pos)
{
    const FileItem &fileitem = *m_fileitem[pos];
    int start = 0;
    int byte_start = 0;
    for (int col : cfg.columns) {
        Cell cell(cfg, fileitem, col);
        cell.col_start = start;
        cell.byte_start = byte_start;
        if (col==FILENAME) {
            string filename(fileitem.filename);
            if (filename.back() != '/' && is_directory(fileitem.fi)) {
                filename.append("/");
            }
            filename.insert(0, cfg.root_marker.c_str());
            cell.text = filename;
        }
        string cell_str(cell.text);
        cell.byte_end = byte_start+cell.text.size();

            cell.col_end = start + cell_str.size();

        // NOTE: alignment
        if (col==FILENAME) {
            int tmp = kStop - cell.col_end;
            // TODO:此处都设置成统一列，在makeline时进行截断
            if (tmp >0) {
                cell.col_end+=tmp;
                cell.byte_end+=tmp;
            }
        }

        int sep = (col==INDENT?0:1);
        start = cell.col_end + sep;
        byte_start = cell.byte_end + sep;

        auto search = col_map.find(col);
        if (search != col_map.end()) {
            col_map[col].insert(search->second.begin()+pos, std::move(cell));
        } else {
            col_map.insert({col, vector<Cell>()});
        }
    }
}
/// pos is 0-based row number.
void Tree::insert_entrylist(const vector<FileItem*>& fil, const int pos, vector<string>& ret)
{
    int file_count = fil.size();
    for (int i = 0; i < file_count; ++i) {
        // string absolute_file_path = file_info.absoluteFilePath();
        // string absolute_file_path(file_info.absoluteFilePath().toUtf8());

        insert_item(pos+i);

        string line;
        makeline(pos+i, line);
        ret.push_back(std::move(line));
    }
}
// TODO: 通过FileItem的parent查询
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

// Checked
/// 0-based [sl, el).
void Tree::hline(int sl, int el)
{
    int bufnr = this->bufnr;
    api->nvim_buf_clear_namespace(bufnr, icon_ns_id, sl, el);
    for (int i = sl;i<el;++i)
    {
        const FileItem &fileitem = *m_fileitem[i];
        char name[40];

        for (const int col : cfg.columns) {
            const Cell & cell = col_map[col][i];

            if(col==FILENAME) {
                sprintf(name, "tree_%u_%u", col, is_directory(fileitem.fi));
                api->async_nvim_buf_add_highlight(bufnr, icon_ns_id, name, i, cell.byte_start, cell.byte_start+cell.text.size());
            } else if(col==ICON || col==GIT || col==MARK) {
                // :hi tree_<tab>
                sprintf(name, "tree_%u_%u", col, cell.color);
                // sprintf(name, "tree_%s", cell.text.data());
                // cout << icon.col_start<< "->"<<icon.col_end;
                // cout << icon.text;
                // auto req_hl = api->nvim_buf_add_highlight(bufnr, 0, "String", 0, 0, 3);
                // call nvim_buf_add_highlight(0, -1, "Identifier", 0, 5, -1)
                api->async_nvim_buf_add_highlight(bufnr, icon_ns_id, name, i, cell.byte_start, cell.byte_start+cell.text.size());
            } else if (col==SIZE || col==TIME ){//|| col==INDENT
                sprintf(name, "tree_%u", col);
                api->async_nvim_buf_add_highlight(bufnr, icon_ns_id, name, i, cell.byte_start, cell.byte_start+cell.text.size());
            }
        }
    }
}

// Checked
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
    copy(directory_iterator(item.p), directory_iterator(), back_inserter(v));
    sort(v.begin(), v.end(), [](path x, path y){return is_directory(x)>is_directory(y);});

    for (auto x : v) {
        FileItem *fileitem = new FileItem;
        fileitem->fi = status(x);
        fileitem->level = level;
        fileitem->parent = &item;
        fileitem->p = x;
        fileitem->filename = x.filename().string();

        // TODO: 临时
        // if (is_directory(x)) {
        //     fileitem->opened_tree = true;
        //     fileitem_lst.push_back(fileitem);
        //     entryInfoListRecursively(*fileitem, fileitem_lst);
        // }
        // else

        auto search = expandStore.find(fileitem->p.string());
        if (search != expandStore.end() && search->second) {
            fileitem->opened_tree = true;
            fileitem_lst.push_back(fileitem);
            entryInfoListRecursively(*fileitem, fileitem_lst);
        }
        else
            fileitem_lst.push_back(fileitem);
    }
}

// Checked
void Tree::expandRecursively(const FileItem &item, vector<FileItem*> &fileitems)
{
    const int level = item.level+1;
    auto it = directory_iterator(item.p);

    for (directory_entry &x : it) {
        FileItem *fileitem = new FileItem;
        fileitem->fi = x.status();
        fileitem->level = level;
        fileitem->parent = &item;
        fileitem->p = x.path();
        fileitem->filename = x.path().filename().string();

        if (is_directory(x)) {
            fileitem->opened_tree = true;
            fileitems.push_back(fileitem);
            entryInfoListRecursively(*fileitem, fileitems);
        }
        else
            fileitems.push_back(fileitem);
    }
}

void Tree::set_last(vector<FileItem *> &fileitems)
{
    unordered_map<int, FileItem *> m;
    for (auto &i : fileitems) {
        m[i->level] = i;
    }
    for (auto &i : m) {
        i.second->last = true;
    }
}

typedef void (Tree::*Action)(const nvim::Array& args);
std::unordered_map<string, Action> action_map {
    {"cd"                   , &Tree::cd},
    // {"goto"                 , &Tree::goto_},
    {"open_or_close_tree"   , &Tree::open_tree},
    // {"open"                 , &Tree::open},
    // {"copy"                 , &Tree::copy},
    // {"move"                 , &Tree::move},
    // {"paste"                , &Tree::pre_paste},
    // {"remove"               , &Tree::pre_remove},
    // {"yank_path"            , &Tree::yank_path},
    // {"toggle_select"        , &Tree::toggle_select},
    // {"toggle_select_all"    , &Tree::toggle_select_all},
    // {"print"                , &Tree::print},
    // {"debug"                , &Tree::debug},
    // {"toggle_ignored_files" , &Tree::toggle_ignored_files},
    // {"redraw"               , &Tree::redraw},
    // {"new_file"             , &Tree::new_file},
    // {"execute_system"       , &Tree::execute_system},
    // {"rename"               , &Tree::rename},
    // {"drop"                 , &Tree::drop},
    // {"call"                 , &Tree::call},
    // {"open_tree_recursive"  , &Tree::open_or_close_tree_recursively},
};
void Tree::action(const string &action, const nvim::Array &args,
                  const Map &context)
{
    cout << __FUNCTION__ << action << endl;

    this->ctx = context;
    cout << "cursor position(1-based): " << ctx.cursor << endl;

    auto search = action_map.find(action);
    if (search != action_map.end()) {
        (this->*action_map[action])(args);
    }
    else {
        api->nvim_call_function("tree#util#print_message", {"Unknown Action: " + action});
    }
}

/// l is 0-based row number.
void Tree::open_tree(const nvim::Array &args)
{
    const int l = ctx.cursor - 1;
    cout << __FUNCTION__ << endl;
    assert(0 <= l && l < m_fileitem.size());
    // if (l == 0) return;
    vector<string> ret;
    FileItem &cur = *m_fileitem[l];

    if (is_directory(cur.fi) && !cur.opened_tree) {

        cur.opened_tree = true;
        const string & rootPath = cur.p.string();
        expandStore.insert({rootPath, true});
        // redraw_line(l, l + 1);
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
        std::cout << "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee" << std::endl;
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
        printf("\tclose range(1-based): [%d, %d]", s+1, e);
        buf_set_lines(s, e, true, {});

        erase_entrylist(s, e);
        cur.opened_tree = false;
        // redraw_line(l, l + 1);
    }
    else if (find_parent(l) >= 0) {
        int parent = find_parent(l);
        std::tuple<int, int> se = find_range(parent);
        int s = std::get<0>(se) + 1;
        int e = std::get<1>(se) + 1;

        printf("\tclose range(1-based): [%d, %d]", s+1, e);

        buf_set_lines(s, e, true, {});
        // ref to https://github.com/equalsraf/neovim-qt/issues/596
        api->nvim_win_set_cursor(0, {0, s});
        erase_entrylist(s, e);

        FileItem &father = *m_fileitem[parent];
        father.opened_tree = false;
        const string & p = father.p.string();
        // if (expandStore.contains(p) && expandStore[p]) {
        //     expandStore[p] = false;
        // }
        // redraw_line(parent, parent + 1);
    }
    return;
}

void Tree::cd(const nvim::Array &args)
{
    if (args.size()>0) {
        string dir = args.at(0).as_string();

        // FIXME 第一次cd无效
        if (dir=="..") {
            path & curdir = m_fileitem[0]->p;
            cout << __FUNCTION__ << curdir.parent_path().string() << endl;
            changeRoot(curdir.parent_path().string());
        }
        else if (dir == ".") {
            FileItem &cur = *m_fileitem[ctx.cursor - 1];
            string dir(cur.p.string());
            // if (cur.opened_tree)
            //     dir = cur.fi.absoluteFilePath();
            string cmd = "cd " + dir;
            api->async_nvim_call_function("tree#util#print_message", {cmd});
            api->nvim_command(cmd);
        }
        else {
            changeRoot(dir);
        }
    }
    else {
        // changeRoot(QDir::home().absolutePath());
    }
}
