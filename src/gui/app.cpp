#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QDebug>
#include <cinttypes>
#include "auto/neovimapi6.h"
#include "neovimconnector.h"
#include "msgpackrequest.h"
#include "app.h"

using std::string;
namespace NeovimQt {

/// A log handler for Qt messages, all messages are dumped into the file
/// passed via the NVIM_QT_LOG variable. Some information is only available
/// in debug builds (e.g. qDebug is only called in debug builds).
///
/// In UNIX Qt prints messages to the console output, but in Windows this is
/// the only way to get Qt's debug/warning messages.
void logger(QtMsgType type, const QMessageLogContext& ctx, const QString& msg)
{
	QFile logFile(qgetenv("NVIM_QT_LOG"));
	if (logFile.open(QIODevice::Append | QIODevice::Text)) {
		QTextStream stream(&logFile);
		stream << msg << "\n";
	}
}

#ifdef Q_OS_MAC
bool getLoginEnvironment(const QString& path)
{
	QProcess proc;
	proc.start(path, {"-l", "-c", "env", "-i"});
	if (!proc.waitForFinished()) {
		qDebug() << "Failed to execute shell to get environemnt" << path;
		return false;
	}

	QByteArray out = proc.readAllStandardOutput();
	foreach(const QByteArray& item, out.split('\n')) {
		int index = item.indexOf('=');
		if (index > 0) {
			qputenv(item.mid(0, index), item.mid(index+1));
			qDebug() << item.mid(0, index) << item.mid(index+1);
		}
	}
	return true;
}
#endif

App::App(int &argc, char ** argv)
:QCoreApplication(argc, argv)
{
	if (!qgetenv("NVIM_QT_LOG").isEmpty()) {
		qInstallMessageHandler(logger);
	}
}

void App::init(NeovimConnector *c)
{
    this->m_nvim = c;
    m_nvim->setRequestHandler(this);

    connect(m_nvim, &NeovimConnector::ready, this, &App::init_channel);
}

void App::init_channel()
{
    char format[] = "%s: %" PRIu64;
    qDebug(format, __PRETTY_FUNCTION__, m_nvim->channel());

    // call rpcnotify(g:tree#_channel_id, "_tree_start", "/Users/zgp/")
    NeovimQt::NeovimApi6 *b = m_nvim->api6();
    b->vim_set_var("tree#_channel_id", int(m_nvim->channel()));
    connect(m_nvim->api6(), &NeovimApi6::neovimNotification, this, &App::handleNeovimNotification);
}

void App::handleHLfinish(quint32 msgid, quint64 fun, const QVariant& val)
{
    int ns_id = val.toInt();
    resource.insert("ns_id", ns_id);
    qDebug() << __PRETTY_FUNCTION__ << ns_id;
}
void App::handleBufReq(quint32 msgid, quint64 fun, const QVariant& val)
{
    int bufnr = val.toInt();
    resource.insert("bufnr", int(bufnr));
    qDebug() << __PRETTY_FUNCTION__ << "bufnr:"<< bufnr;
    // FIXME: 保证在所有资源申请成功后创建tree.
    //  idea: 在所有的handle函数中判断资源是否申请完成(resource是否符合要求)
    createTree();
}

void App::handleNeovimNotification(const QByteArray &method, const QVariantList &args) {
    qDebug() << __PRETTY_FUNCTION__ << method;

    QList<QVariant>	vl = args.at(0).toList();
    qDebug() << args;
    if(method=="_tree_async_action" && args.size() > 0) {
        // _tree_async_action [action: String, args: List, context: Dictionary]
        QString action = vl.at(0).toString();
        QList<QVariant> act_args = vl.at(1).toList();
        QMap<QString, QVariant> context = vl.at(2).toMap();
        qDebug() << "action args:"<< action << act_args;
        // qDebug() << "context:" << context;
        m_ctx = context;
        qDebug()<<trees<< m_ctx.prev_bufnr;
        if (action == "quit" && args.size() > 0) {
            // Tree *tree = trees[ctx.prev_bufnr];
            // trees.remove(ctx.prev_bufnr);
            // delete tree;
        } else{
            trees[m_ctx.prev_bufnr]->action(action, act_args, context);
        }
    }
}

void App::createTree()
{
    int bufnr = resource["bufnr"].toInt();
    int ns_id = resource["ns_id"].toInt();
    QString path = resource["start_path"].toString();
    qDebug() << __PRETTY_FUNCTION__ << "bufnr:"<<bufnr<<"ns_id:"<<ns_id<<path;
    // TODO: 当bufnr不存在时, 回收tree
    Tree *tree = new Tree(bufnr, ns_id, m_nvim);
    trees.insert(bufnr, tree);
    NeovimQt::NeovimApi6 *b = m_nvim->api6();
    tree->cfg.update(m_cfgmap);
    // Record operation bufnr
    m_ctx.prev_bufnr = bufnr;
    tree->changeRoot(path);

    // call nvim_buf_add_highlight(0, src, "Identifier", 0, 5, -1)
    b->nvim_command("set nu");
    b->nvim_command("set nowrap");
    b->nvim_command("set nolist");
    b->nvim_buf_set_option(bufnr, "ft", "tree");
    b->nvim_buf_set_option(bufnr, "modifiable", false);
    qDebug() << "\t"<< QString(tree->cfg.split.c_str());
    char cmd[128];
    sprintf(cmd, "silent keepalt %s %s %s %d", "leftabove", "vertical", "sbuffer", bufnr);
    // sprintf(cmd, "silent keepalt %s %d", "buffer", bufnr);
    b->nvim_command(cmd);
}

void App::handleRequest(MsgpackIODevice* dev, quint32 msgid, const QByteArray& method, const QVariantList& args)
{
    qDebug() << __PRETTY_FUNCTION__<< method << args;

    QList<QVariant>	vl = args[0].toList();
    QList<QVariant> method_args = vl[0].toList();
    QMap<QString, QVariant> context = vl[1].toMap();

    if(method=="_tree_start" && args.size() > 0)
    {
        // _tree_start [paths: List, context: Dictionary]
        QString path = method_args[0].toString();
        NeovimQt::NeovimApi6 *b = m_nvim->api6();
        m_cfgmap = context;
        if (trees.size()<1 || (m_cfgmap.contains("new") && m_cfgmap["new"].toBool()))
        {
            resource.insert("start_path", path);
            auto req_hl = b->nvim_create_namespace("tree_icon");
            // FIXME: use on_ instead
            connect(req_hl, &MsgpackRequest::finished, this, &App::handleHLfinish);
            auto req_buf = b->nvim_create_buf(false, true);
            connect(req_buf, &MsgpackRequest::finished, this, &App::handleBufReq);

        } else {
            // NOTE: Resume tree buffer by default.
            // Tree * tree = *trees.begin();
            QByteArray bufnrs("{");
            foreach(const int bufnr, trees.keys()) {
                bufnrs.append(QString::number(bufnr));
                bufnrs.append(",");
            }
            bufnrs.append("}");
            qDebug()<<bufnrs;
            b->nvim_command("lua require('tree')");
            char cmd[128];
            sprintf(cmd, "lua resume(%s)", bufnrs.data());
            b->nvim_command(cmd);
            trees[m_ctx.prev_bufnr]->cfg.update(m_cfgmap);
            // bufwinid(bufname(bufnr))
        }

        dev->sendResponse(msgid, QVariant(), 0);
    } else {
        // be sure to return early or this message will be sent
        dev->sendResponse(msgid, QString("Unknown method"), QVariant());
    }
}
bool App::event(QEvent *event)
{
	return QCoreApplication::event(event);
}

/// Initialize CLI parser with all the nvim-qt options, process the
/// provided arguments and check for errors.
///
/// When appropriate this function will call QCommandLineParser::showHelp()
/// terminating the program.
void App::processCliOptions(QCommandLineParser &parser, const QStringList& arguments)
{
	parser.addOption(QCommandLineOption("nvim",
				QCoreApplication::translate("main", "nvim executable path"),
				QCoreApplication::translate("main", "nvim_path"),
				"nvim"));
	parser.addOption(QCommandLineOption("timeout",
				QCoreApplication::translate("main", "Error if nvim does not responde after count milliseconds"),
				QCoreApplication::translate("main", "ms"),
				"20000"));

	parser.addOption(QCommandLineOption("embed",
				QCoreApplication::translate("main", "Communicate with Neovim over stdin/out")));
	parser.addOption(QCommandLineOption("server",
				QCoreApplication::translate("main", "Connect to existing Neovim instance"),
				QCoreApplication::translate("main", "addr")));
	parser.addOption(QCommandLineOption("spawn",
				QCoreApplication::translate("main", "Treat positional arguments as the nvim argv")));
	parser.addHelpOption();

#ifdef Q_OS_UNIX
	parser.addOption(QCommandLineOption("nofork",
				QCoreApplication::translate("main", "Run in foreground")));
#endif

	parser.addPositionalArgument("file",
			QCoreApplication::translate("main", "Edit specified file(s)"), "[file...]");
	parser.addPositionalArgument("...", "Additional arguments are forwarded to Neovim", "[-- ...]");

	parser.process(arguments);

	if (parser.isSet("help")) {
		parser.showHelp();
	}

	int exclusive = parser.isSet("server") + parser.isSet("embed") + parser.isSet("spawn");
	if (exclusive > 1) {
		qWarning() << "Options --server, --spawn and --embed are mutually exclusive\n";
		::exit(-1);
	}

	if (!parser.positionalArguments().isEmpty() &&
			(parser.isSet("embed") || parser.isSet("server"))) {
		qWarning() << "--embed and --server do not accept positional arguments\n";
		::exit(-1);
	}

	if (parser.positionalArguments().isEmpty() && parser.isSet("spawn")) {
		qWarning() << "--spawn requires at least one positional argument\n";
		::exit(-1);
	}

	bool valid_timeout;
	int timeout_opt = parser.value("timeout").toInt(&valid_timeout);
	if (!valid_timeout || timeout_opt <= 0) {
		qWarning() << "Invalid argument for --timeout" << parser.value("timeout");
		::exit(-1);
	}
}

NeovimConnector* App::createConnector(const QCommandLineParser& parser)
{
	if (parser.isSet("embed")) {
		return NeovimQt::NeovimConnector::fromStdinOut();
	} else if (parser.isSet("server")) {
		QString server = parser.value("server");
		return NeovimQt::NeovimConnector::connectToNeovim(server);
	} else{
		return nullptr;
	}
}

} // namespace NeovimQt
