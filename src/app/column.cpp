#include "column.h"
#include <QProcess>
#include <QDebug>
#include <QDir>

using namespace std;
FileItem::FileItem()
{
}
Cell::Cell()
{
}
Cell::Cell(const Config &cfg, const FileItem& fileitem, const QString type)
{
    if (type=="mark") {
        color = gui_colors["blue"];
        if (fileitem.fi.permission(QFile::WriteUser))
            text = " ";
        else
            text = mark_indicators["readonly_icon"];
    }
    else if (type == "indent") {
        // NOTE: text="" when level<0.
        // text = QByteArray(fileitem.level*2, ' ');
        int margin = cfg.columns.indexOf("icon")-cfg.columns.indexOf("indent")-1;
        QString prefix(margin*2, ' ');
        text.clear();
        const FileItem* pf = fileitem.parent;
        // from high level to low
        if (fileitem.level>0) {
            if (fileitem.last )
                text.append("└ ");
            else
                text.append("│ ");

            text.prepend(prefix.toUtf8());
            for (int i = 0; i<fileitem.level-1; ++i, pf=pf->parent){
                if(pf->last)
                    text.prepend("  ");
                else
                    text.prepend("│ ");
                text.prepend(prefix.toUtf8());
            }
        }
    }
    else if (type == "git") {
        update_git(fileitem.fi);
    }
    else if (type == "icon") {
        update_icon(fileitem);
    }
    else if (type == "filename") {
        color = gui_colors["yellow"];
        QByteArray filename(fileitem.fi.fileName().toUtf8());
        if (fileitem.fi.isDir()) {
            filename.append("/");
            color = gui_colors["blue"];
        }
        text = filename;
    }
    else if (type == "size") {
        update_size(fileitem.fi);
    } else{
    }
}
Cell::~Cell()
{
}

QMap<QString, QByteArray> FileItem::git_map;
void Cell::update_git(const QFileInfo &fi)
{
    text = " ";
    QString path = fi.absoluteFilePath();
    // qDebug() << "query:" << path;
    if (FileItem::git_map.contains(path))
        text = FileItem::git_map.value(path);
}
void Cell::update_size(const QFileInfo &fi)
{
    auto sz = fi.size();

    char text[8];
    if (0 <= sz && sz < 1024) {
        sprintf(text, "%4lld  B", sz);
    }
    else if (1024 <= sz && sz < 1024 * 1024) {
        sz >>= 10;
        sprintf(text, "%4lld KB", sz);
    }
    else if (1024 * 1024 <= sz && sz < 1024 * 1024 * 1024) {
        sz >>= 20;
        sprintf(text, "%4lld MB", sz);
    }
    else if (1024 * 1024 * 1024 <= sz && sz < 1024LL * 1024 * 1024 * 1024) {
        sz >>= 30;
        sprintf(text, "%4lld GB", sz);
    }
    else if (1024LL * 1024 * 1024 * 1024 <= sz &&
             sz < 1024LL * 1024 * 1024 * 1024 * 1024) {
        sz >>= 40;
        sprintf(text, "%4lld TB", sz);
    }
    else {
    }

    // The size of the directory has no meaning.
    if (!fi.isDir()) this->text = text;
    this->color = gui_colors["green"];
}

void Cell::update_icon(const FileItem & fn)
{
    const QFileInfo &fi = fn.fi;
    QString suffix = fi.suffix();

    auto search = extensions.find(suffix.toStdString());

    if (fi.isDir()){
        if (fn.opened_tree) {
            text = "";
            color = gui_colors["yellow"];
        } else if (fi.isSymLink()){
            // directory_symlink_icon ''
            text = "";
            color = gui_colors["darkBlue"];
        } else {
            // directory_icon ''
            text = "";
            color = gui_colors["pink"];
        }
    } else if (search != extensions.end()) {
        text = icons[search->second][0].toUtf8();
        color = icons[search->second][1];
    } else {
        // default_icon ''
        text = "";
        color = gui_colors["white"];
    }
}
/// Ref to https://git-scm.com/docs/git-status
void get_indicator_name(const QChar X, const QChar Y, string &status)
{
    if (X == '?' and Y == '?')
        status = "Untracked";
    else if (X == ' ' and Y == 'M')
        status = "Modified";
    else if (X == 'M' or X == 'A' or X == 'C')
        status = "Staged";
    else if (X == 'R') // todo
        status = "Renamed";
    else if (X == '!')
        status = "Ignored";
    else if (X == 'U' or Y == 'U' or (X == 'A' and Y == 'A') or
             (X == 'D' and Y == 'D'))
        status = "Unmerged"; // all included
    else if (Y == 'D')
        status = "Deleted";
    else
        status = "Unknown";
}

void FileItem::update_gmap(QString p)
{
    // QString p = "~/OJ";  // ~ cant work
    // QProcess::execute("git rev-parse --show-toplevel");
    FileItem::git_map.clear();
    QProcess proc;
    QString exec = "git";
    QStringList params;
    params << "-C" << p << "rev-parse"
           << "--show-toplevel";
    proc.start(exec, params);
    // sets current thread to sleep and waits for proc end
    proc.waitForFinished();
    QString topdir(proc.readAllStandardOutput());
    topdir.remove(topdir.size()-1, 1);
    // topdir = topdir.trimmed();
    // qDebug() << "input:" << p << "topdir:" << topdir;

    char cmd[200];
    sprintf(cmd, "git -C %s status --porcelain -u", p.toStdString().c_str());
    // qDebug() << "git cmd:" << cmd;
    proc.start(cmd);
    proc.waitForFinished();
    QString res(proc.readAllStandardOutput());

    QStringList lst = res.split("\n");
    // qDebug().noquote()<<lst;
    QDir top_dir(topdir);
    for (int i = 0; i < lst.size(); ++i) {
        QString & line = lst[i];
        if (line.size()<4) continue;

        QChar X=line[0], Y=line[1];
        // QStringRef line(&lst[i], 3, line.length() - 3);
        QString remain = line.mid(3);
        QStringList LR = remain.split(" -> ");
        // TODO: how to reverse escape
        // qDebug()<<"LR"<<LR;

        string status;
        get_indicator_name(X, Y, status);
        // TODO: Check compatible in Windows
        QString key = top_dir.filePath(status == "Renamed" ? LR[1] : LR[0]);
        QDir keyDir(key);
        while (keyDir!=top_dir) {
            FileItem::git_map[keyDir.absolutePath()] = git_indicators[status];
            keyDir.cdUp();
        }
    }
    foreach (const QString &name, git_map.keys())
        qDebug().noquote() << name << ":" << git_map.value(name);

    proc.close();
}

Context::Context(const QMap<QString, QVariant> &ctx)
{
    foreach (const QString &k, ctx.keys()) {
        auto &v = ctx.value(k);
        if (k == "prev_bufnr") {
            prev_bufnr = v.toInt();
        }else if (k == "cursor") {
            cursor = v.toInt();
        }else if (k == "prev_winid") {
            prev_winid = v.toInt();
        }else if (k == "visual_start") {
            visual_start = v.toInt();
        }else if (k == "visual_end") {
            visual_end = v.toInt();
        }
        else{
            qDebug()<<"Unsupported member:"<<k;
        }
    }
}

Config::Config(const QMap<QString, QVariant> &ctx) {
    update(ctx);
}
void Config::update(const QMap<QString, QVariant> &ctx)
{
    foreach (const QString &k, ctx.keys()) {
        auto &v = ctx.value(k);
        if (k == "auto_recursive_level") {
            auto_recursive_level = v.toInt();
        }
        else if (k == "wincol") {
            wincol = v.toInt();
        }
        else if (k == "winheight") {
            winheight = v.toInt();
        }
        else if (k == "winrow") {
            winrow = v.toInt();
        }
        else if (k == "winwidth") {
            winwidth = v.toInt();
        }
        else if (k == "auto_cd") {
            auto_cd = v.toBool();
        }
        else if (k == "listed") {
            listed = v.toBool();
        }
        else if (k == "new") {
            new_ = v.toBool();
        }
        else if (k == "profile") {
            profile = v.toBool();
        }
        else if (k == "resume") {
            resume = v.toBool();
        }
        else if (k == "show_ignored_files") {
            show_ignored_files = v.toBool();
        }
        else if (k == "toggle") {
            toggle = v.toBool();
        }
        else if (k == "root_marker") {
            root_marker = v.toString().toStdString();
        }
        else if (k == "buffer_name") {
            buffer_name = v.toString().toStdString();
        }
        else if (k == "direction") {
            direction = v.toString().toStdString();
        }
        else if (k == "ignored_files") {
            ignored_files = v.toString().toStdString();
        }
        else if (k == "search") {
            search = v.toString().toStdString();
        }
        else if (k == "session_file") {
            session_file = v.toString().toStdString();
        }
        else if (k == "sort") {
            sort = v.toString().toStdString();
        }
        else if (k == "winrelative") {
            winrelative = v.toString().toStdString();
        }
        else if (k == "split") {
            split = v.toString().toStdString();
        }
        else if (k == "columns") {
            columns = v.toString().split(":");
        }
        else{
            qDebug()<<"Unsupported member:"<<k;
        }
    }
}

unordered_map<string, QByteArray> mark_indicators = {
    {"readonly_icon", "✗"},
    {"selected_icon", "✓"},
};

unordered_map<string, QByteArray> git_indicators =  {
    {"Modified"  , "✹" },
    {"Staged"    , "✚" },
    {"Untracked" , "✭" },
    {"Renamed"   , "➜" },
    {"Unmerged"  , "═" },
    {"Ignored"   , "☒" },
    {"Deleted"   , "✖" },
    {"Unknown"   , "?" },
};
// clang-format off
// TODO: QStringLiteral
unordered_map<string, QString> gui_colors =  {
    { "brown"        , "#905532" },
    { "aqua"         , "#3AFFDB" },
    { "blue"         , "#689FB6" },
    { "darkBlue"     , "#44788E" },
    { "purple"       , "#834F79" },
    { "lightPurple"  , "#834F79" },
    { "red"          , "#AE403F" },
    { "beige"        , "#F5C06F" },
    { "yellow"       , "#F09F17" },
    { "orange"       , "#D4843E" },
    { "darkOrange"   , "#F16529" },
    { "pink"         , "#CB6F6F" },
    { "salmon"       , "#EE6E73" },
    { "green"        , "#8FAA54" },
    { "lightGreen"   , "#31B53E" },
    { "white"        , "#FFFFFF" }
};

unordered_map<string, array<QString, 2>> icons = {
    { "folderClosed"    , {"" , "#00afaf"} },
    { "folderOpened"    , {"" , "#00afaf"} },
    { "folderSymlink"   , {"" , "#00afaf"} },
    { "file"            , {"" , "#999999"} },
    { "fileSymlink"     , {"" , "#999999"} },
    { "fileHidden"      , {"﬒" , "#999999"} },
    { "excel"           , {"" , "#207245"} },
    { "word"            , {"" , "#185abd"} },
    { "ppt"             , {"" , "#cb4a32"} },
    { "stylus"          , {"" , "#8dc149"} },
    { "sass"            , {"" , "#f55385"} },
    { "html"            , {"" , "#e37933"} },
    { "xml"             , {"謹", "#e37933"} },
    { "ejs"             , {"" , "#cbcb41"} },
    { "css"             , {"" , "#519aba"} },
    { "webpack"         , {"ﰩ" , "#519aba"} },
    { "markdown"        , {"" , "#519aba"} },
    { "json"            , {"" , "#cbcb41"} },
    { "javascript"      , {"" , "#cbcb41"} },
    { "javascriptreact" , {"" , "#519aba"} },
    { "ruby"            , {"" , "#cc3e44"} },
    { "php"             , {"" , "#a074c4"} },
    { "python"          , {"" , "#519aba"} },
    { "coffee"          , {"" , "#cbcb41"} },
    { "mustache"        , {"" , "#e37933"} },
    { "conf"            , {"" , "#6d8086"} },
    { "image"           , {"" , "#a074c4"} },
    { "ico"             , {"" , "#cbcb41"} },
    { "twig"            , {"" , "#8dc149"} },
    { "c"               , {"" , "#519aba"} },
    { "h"               , {"" , "#a074c4"} },
    { "haskell"         , {"" , "#a074c4"} },
    { "lua"             , {"" , "#519aba"} },
    { "java"            , {"" , "#cc3e44"} },
    { "terminal"        , {"" , "#4d5a5e"} },
    { "ml"              , {"λ" , "#e37933"} },
    { "diff"            , {"" , "#41535b"} },
    { "sql"             , {"" , "#f55385"} },
    { "clojure"         , {"" , "#8dc149"} },
    { "edn"             , {"" , "#519aba"} },
    { "scala"           , {"" , "#cc3e44"} },
    { "go"              , {"" , "#519aba"} },
    { "dart"            , {"" , "#03589C"} },
    { "firefox"         , {"" , "#e37933"} },
    { "vs"              , {"" , "#854CC7"} },
    { "perl"            , {"" , "#519aba"} },
    { "rss"             , {"" , "FB9D3B"} } ,
    { "fsharp"          , {"" , "#519aba"} },
    { "rust"            , {"" , "#519aba"} },
    { "dlang"           , {"" , "#cc3e44"} },
    { "erlang"          , {"" , "#A90533"} },
    { "elixir"          , {"" , "#a074c4"} },
    { "mix"             , {"" , "#cc3e44"} },
    { "vim"             , {"" , "#019833"} },
    { "ai"              , {"" , "#cbcb41"} },
    { "psd"             , {"" , "#519aba"} },
    { "psb"             , {"" , "#519aba"} },
    { "typescript"      , {"" , "#519aba"} },
    { "typescriptreact" , {"" , "#519aba"} },
    { "julia"           , {"" , "#a074c4"} },
    { "puppet"          , {"" , "#cbcb41"} },
    { "vue"             , {"﵂" , "#8dc149"} },
    { "swift"           , {"" , "#e37933"} },
    { "gitconfig"       , {"" , "#41535b"} },
    { "bashrc"          , {"" , "#4d5a5e"} },
    { "favicon"         , {"" , "#cbcb41"} },
    { "docker"          , {"" , "#519aba"} },
    { "gruntfile"       , {"" , "#e37933"} },
    { "gulpfile"        , {"" , "#cc3e44"} },
    { "dropbox"         , {"" , "#0061FE"} },
    { "license"         , {"" , "#cbcb41"} },
    { "procfile"        , {"" , "#a074c4"} },
    { "jquery"          , {"" , "#1B75BB"} },
    { "angular"         , {"" , "#E23237"} },
    { "backbone"        , {"" , "#0071B5"} },
    { "requirejs"       , {"" , "#F44A41"} },
    { "materialize"     , {"" , "#EE6E73"} },
    { "mootools"        , {"" , "#ECECEC"} },
    { "vagrant"         , {"" , "#1563FF"} },
    { "svg"             , {"ﰟ" , "#FFB13B"} },
    { "font"            , {"" , "#999999"} },
    { "text"            , {"" , "#999999"} },
    { "archive"         , {"" , "#cc3e44"} },
};

unordered_map<string, string> extensions = {
    { "styl", "stylus" },
    { "sass", "sass" },
    { "scss", "sass" },
    { "htm", "html" },
    { "html", "html" },
    { "slim", "html" },
    { "xml", "xml" },
    { "xaml", "xml" },
    { "ejs", "ejs" },
    { "css", "css" },
    { "less", "css" },
    { "md", "markdown" },
    { "mdx", "markdown" },
    { "markdown", "markdown" },
    { "rmd", "markdown" },
    { "json", "json" },
    { "js", "javascript" },
    { "es6", "javascript" },
    { "jsx", "javascriptreact" },
    { "rb", "ruby" },
    { "ru", "ruby" },
    { "php", "php" },
    { "py", "python" },
    { "pyc", "python" },
    { "pyo", "python" },
    { "pyd", "python" },
    { "coffee", "coffee" },
    { "mustache", "mustache" },
    { "hbs", "mustache" },
    { "config", "conf" },
    { "conf", "conf" },
    { "ini", "conf" },
    { "yml", "conf" },
    { "yaml", "conf" },
    { "toml", "conf" },
    { "jpg", "image" },
    { "jpeg", "image" },
    { "bmp", "image" },
    { "png", "image" },
    { "gif", "image" },
    { "ico", "ico" },
    { "twig", "twig" },
    { "cpp", "c" },
    { "c++", "c" },
    { "cxx", "c" },
    { "cc", "c" },
    { "cp", "c" },
    { "c", "c" },
    { "h", "h" },
    { "hpp", "h" },
    { "hxx", "h" },
    { "hs", "haskell" },
    { "lhs", "haskell" },
    { "lua", "lua" },
    { "java", "java" },
    { "jar", "java" },
    { "sh", "terminal" },
    { "fish", "terminal" },
    { "bash", "terminal" },
    { "zsh", "terminal" },
    { "ksh", "terminal" },
    { "csh", "terminal" },
    { "awk", "terminal" },
    { "ps1", "terminal" },
    { "bat", "terminal" },
    { "cmd", "terminal" },
    { "ml", "ml" },
    { "mli", "ml" },
    { "diff", "diff" },
    { "db", "sql" },
    { "sql", "sql" },
    { "dump", "sql" },
    { "accdb", "sql" },
    { "clj", "clojure" },
    { "cljc", "clojure" },
    { "cljs", "clojure" },
    { "edn", "edn" },
    { "scala", "scala" },
    { "go", "go" },
    { "dart", "dart" },
    { "xul", "firefox" },
    { "sln", "vs" },
    { "suo", "vs" },
    { "pl", "perl" },
    { "pm", "perl" },
    { "t", "perl" },
    { "rss", "rss" },
    { "f#", "fsharp" },
    { "fsscript", "fsharp" },
    { "fsx", "fsharp" },
    { "fs", "fsharp" },
    { "fsi", "fsharp" },
    { "rs", "rust" },
    { "rlib", "rust" },
    { "d", "dlang" },
    { "erl", "erlang" },
    { "hrl", "erlang" },
    { "ex", "elixir" },
    { "exs", "elixir" },
    { "exx", "elixir" },
    { "leex", "elixir" },
    { "vim", "vim" },
    { "ai", "ai" },
    { "psd", "psd" },
    { "psb", "psd" },
    { "ts", "typescript" },
    { "tsx", "javascriptreact" },
    { "jl", "julia" },
    { "pp", "puppet" },
    { "vue", "vue" },
    { "swift", "swift" },
    { "xcplayground", "swift" },
    { "svg", "svg" },
    { "otf", "font" },
    { "ttf", "font" },
    { "fnt", "font" },
    { "txt", "text" },
    { "text", "text" },
    { "zip", "archive" },
    { "tar", "archive" },
    { "gz", "archive" },
    { "gzip", "archive" },
    { "rar", "archive" },
    { "7z", "archive" },
    { "iso", "archive" },
    { "doc", "word" },
    { "docx", "word" },
    { "docm", "word" },
    { "csv", "excel" },
    { "xls", "excel" },
    { "xlsx", "excel" },
    { "xlsm", "excel" },
    { "ppt", "ppt" },
    { "pptx", "ppt" },
    { "pptm", "ppt" },
};

unordered_map<string, string> filenames = {
    { "gruntfile", "gruntfile" },
    { "gulpfile", "gulpfile" },
    { "gemfile", "ruby" },
    { "guardfile", "ruby" },
    { "capfile", "ruby" },
    { "rakefile", "ruby" },
    { "mix", "mix" },
    { "dropbox", "dropbox" },
    { "vimrc", "vim" },
    { ".vimrc", "vim" },
    { ".gvimrc", "vim" },
    { "_vimrc", "vim" },
    { "_gvimrc", "vim" },
    { "license", "license" },
    { "procfile", "procfile" },
    { "Vagrantfile", "vagrant" },
    { "docker-compose.yml", "docker" },
    { ".gitconfig", "gitconfig" },
    { ".gitignore", "gitconfig" },
    { "webpack", "webpack" },
    { ".bashrc", "bashrc" },
    { ".zshrc", "bashrc" },
    { ".bashprofile", "bashrc" },
    { "favicon.ico", "favicon" },
    { "dockerfile", "docker" },
};

unordered_map<string, string> patternMatches = {
    { ".*jquery.*.js$", "jquery" },
    { ".*angular.*.js$", "angular" },
    { ".*backbone.*.js$", "backbone" },
    { ".*require.*.js$", "requirejs" },
    { ".*materialize.*.js$", "materialize" },
    { ".*materialize.*.css$", "materialize" },
    { ".*mootools.*.js$", "mootools" },
};
// clang-format on

