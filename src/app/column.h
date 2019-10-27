#ifndef NEOVIM_QT_ICON_H
#define NEOVIM_QT_ICON_H
#include <unordered_map>
#include <string>
#include <QFileInfo>
#include <QByteArrayList>
#include <QMap>
#include <QString>
#include <QVariant>
using std::string;
using std::unordered_map;
extern unordered_map<string, unordered_map<string, QString>> extensions;
extern unordered_map<string, QByteArray> git_indicators;
extern unordered_map<string, QByteArray> mark_indicators;
extern unordered_map<string, QString> gui_colors;

class Cell;
class Config;
class FileItem
{
public:
    FileItem();
    virtual ~FileItem(){};

    QFileInfo fi;
    int level = 0;
    bool opened_tree = false;
    // TODO: parent被选中 与 部分child被选中，两者是冲突的 <04-10-19, zgp>
    bool selected = false;
    // TODO: 更快地查找父FileItem
    const FileItem* parent;
    bool last = false;
    static QMap<QString, QByteArray> git_map;
    static void update_gmap(QString p);
};

/// 多个column类意义不大，管理困难
class Cell
{
public:
    Cell();
    Cell(const Config&, const FileItem&, const QString);
    virtual ~Cell();

    // TODO: 考虑添加highlight_id, 并在highlight时加上列作用域防止冲突,
    //  icon/git/mark可以由text作为id,
    //  而filename以文件种类作为id, size以大小种类作为id, ...
    int col_start, col_end;
    // TODO: use text.size() when byte highlight, consider remove byte_end
    int byte_start, byte_end;
    // TODO: QString的size与视觉一致, 而QByteArray不是
    QByteArray text;
    QString color;
    QString tcolor;
    void update_git(const QFileInfo &fi);
    void update_icon(const FileItem & fn);
    void update_size(const QFileInfo&fi);
};

struct Context
{
    // FIXME: 默认构造的成员初始值没有意义
    Context(){};
    Context(const QMap<QString, QVariant> &ctx);
    int cursor = 0;
    QList<QString> drives;
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
    Config(const QMap<QString, QVariant> &ctx);
    virtual ~Config(){};
    void update(const QMap<QString, QVariant> &ctx);

    bool auto_cd = false;
    int auto_recursive_level = 0;
    string buffer_name = "default";
    QStringList columns = {"mark","indent","git","icon","filename","size"};
    string direction = "";
    string ignored_files = "";
    bool listed = false;
    bool profile = false;
    bool new_ = false;
    bool resume = false;

    string root_marker = "[in]: ";
    string search = "";
    string session_file = "";
    string sort = "";

    int wincol = 0;
    int winheight = 0;
    string winrelative = "editor";
    int winrow = 0;
    int winwidth = 0;

    bool show_ignored_files = false;
    string split = "no";
    bool toggle = false;

    // targets: typing.List[typing.Dict[str, typing.Any]] = []
};


#endif
