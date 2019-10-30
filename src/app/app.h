#ifndef NEOVIM_QT_APP
#define NEOVIM_QT_APP

#include <QCoreApplication>
#include <QEvent>
#include <QList>
#include <QCommandLineParser>
#include <tuple>
#include "msgpackiodevice.h"
#include "tree.h"

namespace NeovimQt {

class NeovimConnector;
class App: public QCoreApplication, public MsgpackRequestHandler
{
	Q_OBJECT
public:
	App(int &argc, char ** argv);
	bool event(QEvent *event);
	// void showUi(NeovimConnector *c, const QCommandLineParser&);
	static void processCliOptions(QCommandLineParser& p, const QStringList& arguments);
	static NeovimConnector* createConnector(const QCommandLineParser& p);
    void init(NeovimConnector* m_nvim);

	virtual void handleRequest(MsgpackIODevice* dev, quint32 msgid, const QByteArray& method, const QVariantList& args);
    void createTree();

public slots:
	void handleNeovimNotification(const QByteArray &name, const QVariantList& args);

protected slots:
	void init_channel();
    void handleHLfinish(quint32 msgid, quint64 fun, const QVariant& val);
    void handleBufReq(quint32 msgid, quint64 fun, const QVariant& val);

private:
	NeovimConnector *m_nvim;

    Context m_ctx;
    QMap<QString, QVariant> m_cfgmap;

    // FIXME: cant work when string as key
    QHash<QString, QVariant> resource;
    QHash<int, Tree*> trees;
    QList<int> treebufs;  // Recently used order
};

} // Namespace

#endif
