#ifndef NVIM_COLUMN_H
#define NVIM_COLUMN_H
#include <array>
#include <boost/filesystem.hpp>
#include <string>
#include <unordered_map>
#include "msgpack.hpp"  // msgpack::type::variant
using Map = std::multimap<msgpack::type::variant, msgpack::type::variant>;
using boost::filesystem::file_status;
using std::list;
using std::string;
using std::unordered_map;
namespace tree
{
extern unordered_map<string, string> mark_indicators;
extern std::pair<string, string> git_indicators[];
extern std::pair<string, string> icons[];
extern string gui_colors[];

enum GUI_COLOR {
    BROWN,
    AQUA,
    BLUE,
    DARKBLUE,
    PURPLE,
    LIGHTPURPLE,
    RED,
    BEIGE,
    YELLOW,
    ORANGE,
    DARKORANGE,
    PINK,
    SALMON,
    GREEN,
    LIGHTGREEN,
    WHITE
};
enum column { MARK, INDENT, GIT, ICON, FILENAME, SIZE, TIME };
const int COLUMN_NR = 7;
enum git_status { Untracked, Modified, Staged, Renamed, Ignored, Unmerged, Deleted, Unknown };

class Cell;
class Config;
class FileItem
{
   private:
    using Path = boost::filesystem::path;

   public:
    FileItem() = delete;
    FileItem(Path p);
    ~FileItem(){};

    Path p;
    string filename;
    int level = 0;
    bool opened_tree = false;
    // TODO: Conflict situation: parent is selected and part of child is selected
    bool selected = false;
    const FileItem *parent = nullptr;
    bool last = false;
    static unordered_map<string, git_status> git_map;
    static void update_gmap(string p);
};

/// 多个column类意义不大，管理困难
class Cell
{
   public:
    Cell();
    Cell(const Config &, const FileItem &, const int);
    ~Cell();

    int col_start, col_end;
    // TODO: use text.size() when byte highlight, consider remove byte_end
    int byte_start, byte_end;
    // TODO: QString的size与视觉不一定一致, 而QByteArray更不是
    string text;
    // TODO: 考虑添加highlight_id, 并在highlight时加上列作用域防止冲突,
    //  icon/git/mark可以由text作为id,
    //  而filename以文件种类作为id, size以大小种类作为id, ...
    int color = 666;  // color id, 不同的列用不同的表存储; 也可以是公共的表, 如gui_color
    string tcolor;
    void update_git(const FileItem &fi);
    void update_icon(const FileItem &fn);
    void update_size(const FileItem &fi);
};

struct Context {
    // FIXME: 默认构造的成员初始值没有意义
    Context(){};
    Context(const Map &ctx);
    int cursor = 0;
    list<string> drives;
    int prev_bufnr = 0;
    int prev_winid = 0;
    int visual_start = 0;
    int visual_end = 0;
};

// NOTE: tree状态参数
class Config
{
   public:
    Config(){};
    Config(const Map &ctx);
    ~Config(){};
    void update(const Map &ctx);

    bool auto_cd = false;
    int auto_recursive_level = 0;
    list<int> columns = {MARK, INDENT, GIT, ICON, FILENAME, SIZE, TIME};
    int margin = 1;  // (INDENT, GIT , ICON)
    string ignored_files = "";
    bool show_ignored_files = false;
    bool profile = false;

    string root_marker = "[in]: ";
    string search = "";
    string session_file = "";
    string sort = "";

    bool listed = false;
    string buffer_name = "default";

    string direction = "";
    string split = "no";  // {"vertical", "horizontal", "no", "tab", "floating"}
    string winrelative = "editor";
    int winheight = 30;
    int winwidth = 50;
    int wincol = 0;
    int winrow = 0;
    bool new_ = false;
    bool toggle = false;

    int filename_colstop = 40;
    string time_format = "%d.%M.%Y";
    // targets: typing.List[typing.Dict[str, typing.Any]] = []
};

}  // namespace tree
#endif
