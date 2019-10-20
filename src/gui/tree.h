#ifndef NEOVIM_QT_TREE
#define NEOVIM_QT_TREE

#include <QList>
#include <tuple>
#include "msgpackiodevice.h"
#include "column.h"

namespace NeovimQt {

class NeovimConnector;

class Tree: public QObject
{
    Q_OBJECT
public:
    Tree();
    Tree(int bufnr, int icon_ns_id, NeovimConnector* m_nvim);
    int bufnr = -1;
    int icon_ns_id = -1;
    Config cfg;
    Context ctx;
    QString input;
	NeovimConnector *m_nvim;
    void changeRoot(QString path);
    void open_tree(int);
    void open(int);
    void cd(const QList<QVariant>& args);
    void toggle_select(const int pos);
    void vim_input(string prompt, string text, string completion, string handle);
    void redraw_line(int sl, int el);
    void redraw_recursively(int l);
    void action(const QString &action, const QList<QVariant> &args, const QMap<QString, QVariant> context);

protected slots:
    void handleRename(const QVariant& val);
    void handleNewFile(const QVariant& val);

private:

    QList<FileItem> m_fileitem;
    QMap<QString, QList<Cell>> col_map;
    QHash<QString, bool> expandStore;

    void hline(int sl, int el);
    int find_parent(int l);
    std::tuple<int, int> find_range(int l);
    void insert_entrylist(const QFileInfoList& fl, const int pos, const int level, QList<QByteArray>& ret);
    void insert_item(const int pos);
    void erase_entrylist(const int s, const int e);
    void dyn_make_line(const int pos, QByteArray &line);
    void entryInfoListRecursively(const QString & path, int level, QFileInfoList& lst, QList<FileItem>& fileitem_lst);

    void open_or_close_tree_recursively(int l);
    void shrinkRecursively(const QString &path);
    void expandRecursively(const QString &path, int level, QFileInfoList &lst, QList<FileItem> &fileitem_lst);
};

} // namespace NeovimQt

#endif
