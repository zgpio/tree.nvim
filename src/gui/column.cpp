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
Cell::Cell(const FileItem& fileitem, const QString type)
{
    if (type=="mark") {
        text = " ";
        color = gui_colors["blue"];
    }
    else if (type == "indent") {
        // text = QByteArray(fileitem.level*2, ' ');
        text.clear();
        for (int i = 0;i<fileitem.level;++i){
            text.append("│  ");
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
        sprintf(text, "%4lld B", sz);
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

        // let s:parent_icon = get(g:, 'defx_icons_parent_icon', '')
        // let s:mark_icon = get(g: , 'defx_icons_mark_icon', '*')
        // let s:root_opened_tree_icon = get(g:, 'defx_icons_root_opened_tree_icon', '')
        // let s:nested_closed_tree_icon = get(g: ,'defx_icons_nested_closed_tree_icon', '')
        // let s:nested_opened_tree_icon = get(g: ,'defx_icons_nested_opened_tree_icon', '')
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
        text = search->second["icon"].toUtf8();
        color = search->second["color"];
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
        QString key;
        get_indicator_name(X, Y, status);
        // TODO: Check compatible in Windows
        key = top_dir.filePath(status == "Renamed" ? LR[1] : LR[0]);
        // qDebug() << "key:" << key << endl;
        FileItem::git_map[key] = git_indicators[status];
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
    { "brown"        , "905532" },
    { "aqua"         , "3AFFDB" },
    { "blue"         , "689FB6" },
    { "darkBlue"     , "44788E" },
    { "purple"       , "834F79" },
    { "lightPurple"  , "834F79" },
    { "red"          , "AE403F" },
    { "beige"        , "F5C06F" },
    { "yellow"       , "F09F17" },
    { "orange"       , "D4843E" },
    { "darkOrange"   , "F16529" },
    { "pink"         , "CB6F6F" },
    { "salmon"       , "EE6E73" },
    { "green"        , "8FAA54" },
    { "lightGreen"   , "31B53E" },
    { "white"        , "FFFFFF" }
};

unordered_map<string, QString> term_colors =  {
    { "brown"       , "130"  },
    { "aqua"        , "66"   },
    { "blue"        , "67"   },
    { "darkBlue"    , "57"   },
    { "purple"      , "60"   },
    { "lightPurple" , "103"  },
    { "red"         , "131"  },
    { "beige"       , "137"  },
    { "yellow"      , "229"  },
    { "orange"      , "208"  },
    { "darkOrange"  , "166"  },
    { "pink"        , "205"  },
    { "salmon"      , "209"  },
    { "green"       , "65"   },
    { "lightGreen"  , "108"  },
    { "white"       , "231"  },
};

unordered_map<string, unordered_map<string, QString>> extensions =  {
    { "styl"    , {{"icon", ""}, {"color" , gui_colors["green"]       }, {"term_color" , term_colors["green"]}       }},
    { "sass"    , {{"icon", ""}, {"color" , gui_colors["white"]       }, {"term_color" , term_colors["white"]}       }},
    { "scss"    , {{"icon", ""}, {"color" , gui_colors["pink"]        }, {"term_color" , term_colors["pink"]}        }},
    { "htm"     , {{"icon", ""}, {"color" , gui_colors["darkOrange"]  }, {"term_color" , term_colors["darkOrange"]}  }},
    { "html"    , {{"icon", ""}, {"color" , gui_colors["darkOrange"]  }, {"term_color" , term_colors["darkOrange"]}  }},
    { "slim"    , {{"icon", ""}, {"color" , gui_colors["orange"]      }, {"term_color" , term_colors["orange"]}      }},
    { "ejs"     , {{"icon", ""}, {"color" , gui_colors["yellow"]      }, {"term_color" , term_colors["yellow"]}      }},
    { "css"     , {{"icon", ""}, {"color" , gui_colors["blue"]        }, {"term_color" , term_colors["blue"]}        }},
    { "less"    , {{"icon", ""}, {"color" , gui_colors["darkBlue"]    }, {"term_color" , term_colors["darkBlue"]}    }},
    { "md"      , {{"icon", ""}, {"color" , gui_colors["yellow"]      }, {"term_color" , term_colors["yellow"]}      }},
    { "markdown", {{"icon", ""}, {"color" , gui_colors["yellow"]      }, {"term_color" , term_colors["yellow"]}      }},
    { "rmd"     , {{"icon", ""}, {"color" , gui_colors["white"]       }, {"term_color" , term_colors["white"]}       }},
    { "json"    , {{"icon", ""}, {"color" , gui_colors["beige"]       }, {"term_color" , term_colors["beige"]}       }},
    { "js"      , {{"icon", ""}, {"color" , gui_colors["beige"]       }, {"term_color" , term_colors["beige"]}       }},
    { "mjs"     , {{"icon", ""}, {"color" , gui_colors["beige"]       }, {"term_color" , term_colors["beige"]}       }},
    { "jsx"     , {{"icon", ""}, {"color" , gui_colors["blue"]        }, {"term_color" , term_colors["blue"]}        }},
    { "rb"      , {{"icon", ""}, {"color" , gui_colors["red"]         }, {"term_color" , term_colors["red"]}         }},
    { "php"     , {{"icon", ""}, {"color" , gui_colors["purple"]      }, {"term_color" , term_colors["purple"]}      }},
    { "py"      , {{"icon", ""}, {"color" , gui_colors["yellow"]      }, {"term_color" , term_colors["yellow"]}      }},
    { "pyc"     , {{"icon", ""}, {"color" , gui_colors["yellow"]      }, {"term_color" , term_colors["yellow"]}      }},
    { "pyo"     , {{"icon", ""}, {"color" , gui_colors["yellow"]      }, {"term_color" , term_colors["yellow"]}      }},
    { "pyd"     , {{"icon", ""}, {"color" , gui_colors["yellow"]      }, {"term_color" , term_colors["yellow"]}      }},
    { "coffee"  , {{"icon", ""}, {"color" , gui_colors["brown"]       }, {"term_color" , term_colors["brown"]}       }},
    { "mustache", {{"icon", ""}, {"color" , gui_colors["orange"]      }, {"term_color" , term_colors["orange"]}      }},
    { "hbs"     , {{"icon", ""}, {"color" , gui_colors["orange"]      }, {"term_color" , term_colors["orange"]}      }},
    { "conf"    , {{"icon", ""}, {"color" , gui_colors["white"]       }, {"term_color" , term_colors["white"]}       }},
    { "ini"     , {{"icon", ""}, {"color" , gui_colors["white"]       }, {"term_color" , term_colors["white"]}       }},
    { "yml"     , {{"icon", ""}, {"color" , gui_colors["white"]       }, {"term_color" , term_colors["white"]}       }},
    { "yaml"    , {{"icon", ""}, {"color" , gui_colors["white"]       }, {"term_color" , term_colors["white"]}       }},
    { "bat"     , {{"icon", ""}, {"color" , gui_colors["white"]       }, {"term_color" , term_colors["white"]}       }},
    { "jpg"     , {{"icon", ""}, {"color" , gui_colors["aqua"]        }, {"term_color" , term_colors["aqua"]}        }},
    { "jpeg"    , {{"icon", ""}, {"color" , gui_colors["aqua"]        }, {"term_color" , term_colors["aqua"]}        }},
    { "bmp"     , {{"icon", ""}, {"color" , gui_colors["aqua"]        }, {"term_color" , term_colors["aqua"]}        }},
    { "png"     , {{"icon", ""}, {"color" , gui_colors["aqua"]        }, {"term_color" , term_colors["aqua"]}        }},
    { "gif"     , {{"icon", ""}, {"color" , gui_colors["aqua"]        }, {"term_color" , term_colors["aqua"]}        }},
    { "ico"     , {{"icon", ""}, {"color" , gui_colors["aqua"]        }, {"term_color" , term_colors["aqua"]}        }},
    { "twig"    , {{"icon", ""}, {"color" , gui_colors["green"]       }, {"term_color" , term_colors["green"]}       }},
    { "cpp"     , {{"icon", ""}, {"color" , gui_colors["blue"]        }, {"term_color" , term_colors["blue"]}        }},
    { "cxx"     , {{"icon", ""}, {"color" , gui_colors["blue"]        }, {"term_color" , term_colors["blue"]}        }},
    { "cc"      , {{"icon", ""}, {"color" , gui_colors["blue"]        }, {"term_color" , term_colors["blue"]}        }},
    { "cp"      , {{"icon", ""}, {"color" , gui_colors["blue"]        }, {"term_color" , term_colors["blue"]}        }},
    { "c"       , {{"icon", ""}, {"color" , gui_colors["blue"]        }, {"term_color" , term_colors["blue"]}        }},
    { "h"       , {{"icon", ""}, {"color" , gui_colors["white"]       }, {"term_color" , term_colors["white"]}       }},
    { "hpp"     , {{"icon", ""}, {"color" , gui_colors["white"]       }, {"term_color" , term_colors["white"]}       }},
    { "hxx"     , {{"icon", ""}, {"color" , gui_colors["white"]       }, {"term_color" , term_colors["white"]}       }},
    { "hs"      , {{"icon", ""}, {"color" , gui_colors["beige"]       }, {"term_color" , term_colors["beige"]}       }},
    { "lhs"     , {{"icon", ""}, {"color" , gui_colors["beige"]       }, {"term_color" , term_colors["beige"]}       }},
    { "lua"     , {{"icon", ""}, {"color" , gui_colors["purple"]      }, {"term_color" , term_colors["purple"]}      }},
    { "java"    , {{"icon", ""}, {"color" , gui_colors["purple"]      }, {"term_color" , term_colors["purple"]}      }},
    { "sh"      , {{"icon", ""}, {"color" , gui_colors["lightPurple"] }, {"term_color" , term_colors["lightPurple"]} }},
    { "fish"    , {{"icon", ""}, {"color" , gui_colors["green"]       }, {"term_color" , term_colors["green"]}       }},
    { "bash"    , {{"icon", ""}, {"color" , gui_colors["white"]       }, {"term_color" , term_colors["white"]}       }},
    { "zsh"     , {{"icon", ""}, {"color" , gui_colors["white"]       }, {"term_color" , term_colors["white"]}       }},
    { "ksh"     , {{"icon", ""}, {"color" , gui_colors["white"]       }, {"term_color" , term_colors["white"]}       }},
    { "csh"     , {{"icon", ""}, {"color" , gui_colors["white"]       }, {"term_color" , term_colors["white"]}       }},
    { "awk"     , {{"icon", ""}, {"color" , gui_colors["white"]       }, {"term_color" , term_colors["white"]}       }},
    { "ps1"     , {{"icon", ""}, {"color" , gui_colors["white"]       }, {"term_color" , term_colors["white"]}       }},
    { "ml"      , {{"icon", "λ"}, {"color" , gui_colors["yellow"]      }, {"term_color" , term_colors["yellow"]}      }},
    { "mli"     , {{"icon", "λ"}, {"color" , gui_colors["yellow"]      }, {"term_color" , term_colors["yellow"]}      }},
    { "diff"    , {{"icon", ""}, {"color" , gui_colors["white"]       }, {"term_color" , term_colors["white"]}       }},
    { "db"      , {{"icon", ""}, {"color" , gui_colors["blue"]        }, {"term_color" , term_colors["blue"]}        }},
    { "sql"     , {{"icon", ""}, {"color" , gui_colors["darkBlue"]    }, {"term_color" , term_colors["darkBlue"]}    }},
    { "dump"    , {{"icon", ""}, {"color" , gui_colors["blue"]        }, {"term_color" , term_colors["blue"]}        }},
    { "clj"     , {{"icon", ""}, {"color" , gui_colors["green"]       }, {"term_color" , term_colors["green"]}       }},
    { "cljc"    , {{"icon", ""}, {"color" , gui_colors["green"]       }, {"term_color" , term_colors["green"]}       }},
    { "cljs"    , {{"icon", ""}, {"color" , gui_colors["green"]       }, {"term_color" , term_colors["green"]}       }},
    { "edn"     , {{"icon", ""}, {"color" , gui_colors["green"]       }, {"term_color" , term_colors["green"]}       }},
    { "scala"   , {{"icon", ""}, {"color" , gui_colors["red"]         }, {"term_color" , term_colors["red"]}         }},
    { "go"      , {{"icon", ""}, {"color" , gui_colors["beige"]       }, {"term_color" , term_colors["beige"]}       }},
    { "dart"    , {{"icon", ""}, {"color" , gui_colors["white"]       }, {"term_color" , term_colors["white"]}       }},
    { "xul"     , {{"icon", ""}, {"color" , gui_colors["darkOrange"]  }, {"term_color" , term_colors["darkOrange"]}  }},
    { "sln"     , {{"icon", ""}, {"color" , gui_colors["purple"]      }, {"term_color" , term_colors["purple"]}      }},
    { "suo"     , {{"icon", ""}, {"color" , gui_colors["purple"]      }, {"term_color" , term_colors["purple"]}      }},
    { "pl"      , {{"icon", ""}, {"color" , gui_colors["blue"]        }, {"term_color" , term_colors["blue"]}        }},
    { "pm"      , {{"icon", ""}, {"color" , gui_colors["blue"]        }, {"term_color" , term_colors["blue"]}        }},
    { "t"       , {{"icon", ""}, {"color" , gui_colors["blue"]        }, {"term_color" , term_colors["blue"]}        }},
    { "rss"     , {{"icon", ""}, {"color" , gui_colors["darkOrange"]  }, {"term_color" , term_colors["darkOrange"]}  }},
    { "fsscript", {{"icon", ""}, {"color" , gui_colors["blue"]        }, {"term_color" , term_colors["blue"]}        }},
    { "fsx"     , {{"icon", ""}, {"color" , gui_colors["blue"]        }, {"term_color" , term_colors["blue"]}        }},
    { "fs"      , {{"icon", ""}, {"color" , gui_colors["blue"]        }, {"term_color" , term_colors["blue"]}        }},
    { "fsi"     , {{"icon", ""}, {"color" , gui_colors["blue"]        }, {"term_color" , term_colors["blue"]}        }},
    { "rs"      , {{"icon", ""}, {"color" , gui_colors["darkOrange"]  }, {"term_color" , term_colors["darkOrange"]}  }},
    { "rlib"    , {{"icon", ""}, {"color" , gui_colors["darkOrange"]  }, {"term_color" , term_colors["darkOrange"]}  }},
    { "d"       , {{"icon", ""}, {"color" , gui_colors["red"]         }, {"term_color" , term_colors["red"]}         }},
    { "erl"     , {{"icon", ""}, {"color" , gui_colors["lightPurple"] }, {"term_color" , term_colors["lightPurple"]} }},
    { "ex"      , {{"icon", ""}, {"color" , gui_colors["lightPurple"] }, {"term_color" , term_colors["lightPurple"]} }},
    { "exs"     , {{"icon", ""}, {"color" , gui_colors["lightPurple"] }, {"term_color" , term_colors["lightPurple"]} }},
    { "eex"     , {{"icon", ""}, {"color" , gui_colors["lightPurple"] }, {"term_color" , term_colors["lightPurple"]} }},
    { "hrl"     , {{"icon", ""}, {"color" , gui_colors["pink"]        }, {"term_color" , term_colors["pink"]}        }},
    { "vim"     , {{"icon", ""}, {"color" , gui_colors["green"]       }, {"term_color" , term_colors["green"]}       }},
    { "ai"      , {{"icon", ""}, {"color" , gui_colors["darkOrange"]  }, {"term_color" , term_colors["darkOrange"]}  }},
    { "psd"     , {{"icon", ""}, {"color" , gui_colors["darkBlue"]    }, {"term_color" , term_colors["darkBlue"]}    }},
    { "psb"     , {{"icon", ""}, {"color" , gui_colors["darkBlue"]    }, {"term_color" , term_colors["darkBlue"]}    }},
    { "ts"      , {{"icon", ""}, {"color" , gui_colors["blue"]        }, {"term_color" , term_colors["blue"]}        }},
    { "tsx"     , {{"icon", ""}, {"color" , gui_colors["white"]       }, {"term_color" , term_colors["white"]}       }},
    { "jl"      , {{"icon", ""}, {"color" , gui_colors["purple"]      }, {"term_color" , term_colors["purple"]}      }},
    { "pp"      , {{"icon", ""}, {"color" , gui_colors["white"]       }, {"term_color" , term_colors["white"]}       }},
    { "vue"     , {{"icon", "﵂"}, {"color" , gui_colors["green"]       }, {"term_color" , term_colors["green"]}       }},
};
// clang-format on

