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
        auto & column = col_map[col];
        auto it = std::next(column.begin(), pos);
        const Cell & cell = *it;
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

    list<FileItem*> child_fileitem;

    auto it = std::next(m_fileitem.begin(), 0);
    entryInfoListRecursively(*(*it), child_fileitem);
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
    auto it = std::next(m_fileitem.begin(), pos);
    const FileItem &fileitem = *(*it);
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
            auto it = std::next(col_map[col].begin(), pos);
            col_map[col].insert(it, std::move(cell));
        } else {
            col_map.insert({col, list<Cell>()});
        }
    }
}

void Tree::insert_rootcell(const int pos)
{
    auto it = std::next(m_fileitem.begin(), pos);
    const FileItem &fileitem = *(*it);
    int start = 0;
    int byte_start = 0;
    for (int col : cfg.columns) {
        Cell cell(cfg, fileitem, col);
        cell.col_start = start;
        cell.byte_start = byte_start;
        if (col==FILENAME) {
            string filename(fileitem.filename);
            if (is_directory(fileitem.fi)) {
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
            auto it = std::next(col_map[col].begin(), pos);
            col_map[col].insert(it, std::move(cell));
        } else {
            col_map.insert({col, list<Cell>()});
        }
    }
}
/// pos is 0-based row number.
void Tree::insert_entrylist(const list<FileItem*>& fil, const int pos, vector<string>& ret)
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
    auto it = std::next(m_fileitem.begin(), l);
    int level = (*it)->level;
    for (int i=l-1;i>=0;--i)
    {
        auto it = std::next(m_fileitem.begin(), i);
        const FileItem &fn = *(*it);
        if (fn.level == level-1)
            return i;
    }
    return -1;
}

/// [l, l] <=> no sub files; l is parent row number(0-based).
std::tuple<int, int> Tree::find_range(int l)
{
    int s=l, i;
    auto it = std::next(m_fileitem.begin(), l);
    int level = (*it)->level;
    for (i=l+1;i<m_fileitem.size();++i)
    {
        auto it = std::next(m_fileitem.begin(), i);
        int l = (*it)->level;
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
        auto it = std::next(m_fileitem.begin(), i);
        const FileItem &fileitem = **it;
        char name[40];

        for (const int col : cfg.columns) {
            auto it = std::next(col_map[col].begin(), i);
            const Cell & cell = *it;

            if(col==FILENAME) {
                sprintf(name, "tree_%u_%u", col, is_directory(fileitem.fi));
                api->nvim_buf_add_highlight(bufnr, icon_ns_id, name, i, cell.byte_start, cell.byte_start+cell.text.size());
            } else if(col==ICON || col==GIT || col==MARK) {
                // :hi tree_<tab>
                sprintf(name, "tree_%u_%u", col, cell.color);
                // sprintf(name, "tree_%s", cell.text.data());
                // cout << icon.col_start<< "->"<<icon.col_end;
                // cout << icon.text;
                // auto req_hl = api->nvim_buf_add_highlight(bufnr, 0, "String", 0, 0, 3);
                // call nvim_buf_add_highlight(0, -1, "Identifier", 0, 5, -1)
                api->nvim_buf_add_highlight(bufnr, icon_ns_id, name, i, cell.byte_start, cell.byte_start+cell.text.size());
            } else if (col==SIZE || col==TIME ){//|| col==INDENT
                sprintf(name, "tree_%u", col);
                api->nvim_buf_add_highlight(bufnr, icon_ns_id, name, i, cell.byte_start, cell.byte_start+cell.text.size());
            }
        }
    }
}

/// erase [s, e)
void Tree::erase_entrylist(const int s, const int e)
{
    for (int col : cfg.columns) {
        list<Cell> & cell = col_map[col];
        auto it = std::next(cell.begin(), s);
        for (int i=0;i<e-s;++i) {
            auto nit = std::next(it);
            cell.erase(it);
            it = nit;
        }
    }
    for (int i = s; i < e; ++i) {
        auto it = std::next(m_fileitem.begin(), i);
        delete (*it);
    }
    auto start = m_fileitem.begin();
    std::advance(start, s);
    auto end = std::next(m_fileitem.begin(), e);
    m_fileitem.erase(start, end);
}

// get entryInfoList recursively
void Tree::entryInfoListRecursively(const FileItem& item,
                                   list<FileItem*> &fileitem_lst)
{
    path dir(item.p);
    const int level = item.level+1;

    for (directory_entry &x : directory_iterator(dir)) {
        FileItem *fileitem = new FileItem;
        fileitem->fi = x.status();
        fileitem->level = level;
        fileitem->parent = &item;
        fileitem->p = x.path();
        fileitem->filename = x.path().filename().string();

        if (is_directory(x)) {
            fileitem->opened_tree = true;
            fileitem_lst.push_back(fileitem);
            entryInfoListRecursively(*fileitem, fileitem_lst);
        }
        else
            fileitem_lst.push_back(fileitem);
    }
}

void Tree::set_last(list<FileItem *> &fileitems)
{
    unordered_map<int, FileItem *> m;
    for (auto &i : fileitems) {
        m[i->level] = i;
    }
    for (auto &i : m) {
        i.second->last = true;
    }
}

typedef void (Tree::*Action)(const list<nvim::Object>& args);
std::unordered_map<string, Action> action_map {
    // {"cd"                   , &Tree::cd},
    // {"goto"                 , &Tree::goto_},
    // {"open_or_close_tree"   , &Tree::open_tree},
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
void Tree::action(const string &action, const list<nvim::Object> &args,
                  const unordered_map<string, nvim::Object> context)
{
    cout << __PRETTY_FUNCTION__ << action;

    // cout << "context:" << context;
    // this->ctx = context;
    // cout << "cursor position(1-based): " << ctx.cursor;
    //
    // if (action_map.contains(action)) {
    //     (this->*action_map[action])(args);
    // }
    // else {
    //     api->nvim_call_function("tree#util#print_message", {QString("Unknown Action: %1").arg(action)});
    // }
}
