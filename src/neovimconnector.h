#ifndef NEOVIM_QT_CONNECTOR
#define NEOVIM_QT_CONNECTOR

#include <QObject>
#include <QAbstractSocket>
#include <QProcess>
#include <QTextCodec>
#include "msgpackiodevice.h"
#include "auto/neovimapi6.h"

namespace NeovimQt {

class MsgpackIODevice;
class MsgpackRequestHandler;
class NeovimConnectorHelper;
class NeovimConnector: public QObject
{
	friend class NeovimApi6;
	friend class NeovimConnectorHelper;
	Q_OBJECT
	/**
	 * True if the Neovim instance is ready
	 * @see neovimObject
	 */
	Q_PROPERTY(bool ready READ isReady NOTIFY ready)
public:
	enum NeovimError {
		NoError=0,
		NoMetadata,
		MetadataDescriptorError,
		UnexpectedMsg,
		APIMisMatch,
		NoSuchMethod,
		FailedToStart,
		Crashed,
		SocketError,
		MsgpackError,
		RuntimeMsgpackError,
	};
#ifdef Q_ENUM
	Q_ENUM(NeovimError)
#else
	Q_ENUMS(NeovimError)
#endif

	/** Underlying connection used to read Neovim */
        enum NeovimConnectionType {
		OtherConnection,
		SpawnedConnection,
		HostConnection,
		SocketConnection,
        };

	NeovimConnector(QIODevice* s);
	NeovimConnector(MsgpackIODevice* s);
	static NeovimConnector* spawn(const QStringList& params=QStringList(),
									const QString& exe="nvim");
	static NeovimConnector* connectToSocket(const QString&);
	static NeovimConnector* connectToHost(const QString& host, int port);
	static NeovimConnector* connectToNeovim(const QString& server=QString());
	static NeovimConnector* fromStdinOut();

	bool canReconnect();
	NeovimConnector* reconnect();

	NeovimError errorCause();
	QString errorString();

	bool isReady();
	NeovimApi6 * api6();
	uint64_t channel();
	QString decode(const QByteArray&);
	QByteArray encode(const QString&);
	NeovimConnectionType connectionType();
	/** Some requests for metadata and ui attachment enforce a timeout in ms */
	void setRequestTimeout(int);
	/** Set a handler for msgpack rpc requests **/
	void setRequestHandler(MsgpackRequestHandler *);

	quint64 apiCompatibility();
	quint64 apiLevel();
	bool hasUIOption(const QByteArray& option);

signals:
	/** Emitted when Neovim is ready @see ready */
	void ready();
	void error(NeovimError);
	void processExited(int exitCode);

public slots:
	void fatalTimeout();

protected:
	void setError(NeovimError err, const QString& msg);
	void clearError();

protected slots:
	void discoverMetadata();
	void processError(QProcess::ProcessError);
	void socketError();
	void msgpackError();

private:
	MsgpackIODevice *m_dev{ nullptr };
	NeovimConnectorHelper *m_helper{ nullptr };
	QString m_errorString;
	NeovimError m_error{ NoError };

	NeovimApi6 *m_api6{ nullptr };
	quint64 m_channel{ 0 };
	quint64 m_api_compat{ 0 };
	quint64 m_api_supported{ 0 };

	// Store connection arguments for reconnect()
	NeovimConnectionType m_ctype{ OtherConnection };
	QStringList m_spawnArgs;
	QString m_spawnExe;
	QString m_connSocket, m_connHost;
	QVariantList m_uiOptions;
	int m_connPort;
	bool m_ready{ false };
	int m_timeout{ 20000 };
};
} // namespace NeovimQt
Q_DECLARE_METATYPE(NeovimQt::NeovimConnector::NeovimError)
Q_DECLARE_METATYPE(int64_t)

#endif
