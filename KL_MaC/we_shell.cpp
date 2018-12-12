#include "we_shell.h"

#include <QLineEdit>  
#include <QTextBlock>  
#include <QDebug>  
#include <QThread>

extern QList<QString> g_disText;

macShell::macShell(QWidget *parent) :
	QPlainTextEdit(parent)
{
	setReadOnly(true);
	setWindowOpacity(0.9);//透明度
	setMinimumSize(600,400);
	setWindowTitle(weChinese2LocalCode("KL-MaC 远程命令行交互工具"));

	QFont font = this->font();
	//font.setPointSize(font.pointSize() + 2);
	font.setPointSize(font.pointSize());
	this->setFont(font);

	appendPlainText(">>> ");
	edit = new QLineEdit(this->viewport());
	edit->setStyleSheet("border-style:none; background-color:transparent;");

	connect(edit, SIGNAL(returnPressed()), SLOT(onEditFinished()));
	connect(verticalScrollBar(), SIGNAL(valueChanged(int)), SLOT(onScrollBarValueChanged()));
}

macShell::~macShell()
{

}

void macShell::resizeEvent(QResizeEvent *e)
{
	updateEditPosition();
}

void macShell::onScrollBarValueChanged()
{
	updateEditPosition();
}

QString macShell::runCommand(const QString &cmd)
{  
	return QString("Result of %1").arg(cmd); 
}

void macShell::saveSshObjInf(QString ip, QString usr, QString pwd)
{
	QString title = weChinese2LocalCode("KL-MaC 远程命令行交互工具");
	title = title + "~" + ip + " : " + usr;
	setWindowTitle(title);

	m_ip = ip;
	m_usr = usr;
	m_pwd = pwd;

	m_pThread = new QThread(this);
	m_pWorker = new QWorker_ssh(this,m_ip,m_usr,m_pwd);
	if (NULL != m_pThread && NULL != m_pWorker)
	{
		connect(this,SIGNAL(giveCmdLine(QString)),m_pWorker,SLOT(Exec(QString)));
		connect(this, SIGNAL(giveRemoteInfo(QString, QString, QString)), m_pWorker, SLOT(remoteInfo(QString, QString, QString)));
		connect(m_pWorker, SIGNAL(sshRemoteResponse(QString)), this, SLOT(remoteResponse(QString)));
		m_pWorker->moveToThread(m_pThread);
		m_pThread->start();
		emit giveRemoteInfo(m_ip, m_usr, m_pwd);
	}

}

void macShell::remoteResponse(QString str)
{
	QString cmd = edit->text();
	//if (cmd.isEmpty()) {
	//	return;
	//}
	moveCursor(QTextCursor::End);
	insertPlainText(cmd);
	edit->hide();
	edit->clear();

	appendPlainText(str);

	appendPlainText(">>> ");
	updateEditPosition();
	edit->show();
	edit->setFocus();
}

void macShell::onEditFinished()
{
	QString cmd = edit->text();
	if (cmd.isEmpty()) {
		return;
	}
	moveCursor(QTextCursor::End);
	insertPlainText(cmd);
	edit->hide();
	edit->clear();

	//appendPlainText(runCommand(cmd));
	emit giveCmdLine(cmd);

	//appendPlainText(">>> ");
	updateEditPosition();
	edit->show();
	edit->setFocus();
}

void macShell::updateEditPosition()
{
	resize(this->size());
	QRect rect = cursorRect();
	int scrollWidth = verticalScrollBar()->width();
	edit->resize(this->width() - rect.topRight().x() - scrollWidth, edit->height());
	edit->move(rect.topRight());

	//QPlainTextEditPrivate * d = reinterpret_cast<QPlainTextEditPrivate*>(qGetPtrHelper(d_ptr));
	//QRectF rect = d->control->blockBoundingRect(d->control->document()->lastBlock());
	//edit->move(rect.topRight().toPoint());
	//edit->resize(viewport()->size().width(), edit->size().height());
}


QWorker_ssh::QWorker_ssh(QObject *parent, QString ip, QString usr, QString pwd)
	: QObject(parent)
{
	r_ip = ip;
	r_usr = usr;
	r_pwd = pwd;

#ifdef WIN32
	WSADATA wsadata;
	int err;

	err = WSAStartup(MAKEWORD(2, 0), &wsadata);
	if (err != 0) {
		return ;
	}
#endif
	QByteArray ba = r_ip.toLatin1();
	hostname = ba.data();

	ba = r_usr.toLatin1();
	username = ba.data();

	ba = r_pwd.toLatin1();
	password = ba.data();

	rc = libssh2_init(0);
	if (rc != 0) {
		g_disText << "libssh2 initialization failed";
		QString tmp = "libssh2 initialization failed";
		emit sshRemoteResponse(tmp);
		return ;
	}

	hostaddr = inet_addr(hostname);
	/* Ultra basic "connect to port 22 on localhost"
	* Your code is responsible for creating the socket establishing the
	* connection
	*/
	sock = socket(AF_INET, SOCK_STREAM, 0);

	sin.sin_family = AF_INET;
	sin.sin_port = htons(22);
	sin.sin_addr.s_addr = hostaddr;
	if (::connect(sock, (struct sockaddr*)(&sin),	sizeof(struct sockaddr_in)) != 0) {
		g_disText << "failed to connect!";
		return ;
	}

	/* Create a session instance */
	session = libssh2_session_init();
	if (!session)
		return ;

	/* tell libssh2 we want it all done non-blocking */
	libssh2_session_set_blocking(session, 0);

	/* ... start it up. This will trade welcome banners, exchange keys,
	* and setup crypto, compression, and MAC layers
	*/
	while ((rc = libssh2_session_handshake(session, sock)) == LIBSSH2_ERROR_EAGAIN);
	if (rc) {
		g_disText << "Failure establishing SSH session";
		return ;
	}

	nh = libssh2_knownhost_init(session);
	if (!nh) {
		/* eeek, do cleanup here */
		return ;
	}

	/* read all hosts from here */
	libssh2_knownhost_readfile(nh, "known_hosts",LIBSSH2_KNOWNHOST_FILE_OPENSSH);

	/* store all known hosts to here */
	libssh2_knownhost_writefile(nh, "dumpfile",	LIBSSH2_KNOWNHOST_FILE_OPENSSH);

	fingerprint = libssh2_session_hostkey(session, &len, &type);
	if (fingerprint) {
		struct libssh2_knownhost *host;
#if LIBSSH2_VERSION_NUM >= 0x010206
		/* introduced in 1.2.6 */
		int check = libssh2_knownhost_checkp(nh, hostname, 22,

			fingerprint, len,
			LIBSSH2_KNOWNHOST_TYPE_PLAIN |
			LIBSSH2_KNOWNHOST_KEYENC_RAW,
			&host);
#else
		/* 1.2.5 or older */
		int check = libssh2_knownhost_check(nh, hostname,fingerprint, len,LIBSSH2_KNOWNHOST_TYPE_PLAIN |LIBSSH2_KNOWNHOST_KEYENC_RAW,&host);
#endif
		//fprintf(stderr, "Host check: %d, key: %s\n", check,
		//	(check <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) ?
		//	host->key : "<none>");

		/*****
		* At this point, we could verify that 'check' tells us the key is
		* fine or bail out.
		*****/
	}
	else {
		/* eeek, do cleanup here */
		return ;
	}
	libssh2_knownhost_free(nh);


	if (strlen(password) != 0) {
		/* We could authenticate via password */
		while ((rc = libssh2_userauth_password(session, username, password)) ==		LIBSSH2_ERROR_EAGAIN);
		if (rc) {
			g_disText << "Authentication by password failed";
			return;
		}
	}
	else {
		/* Or by public key */
		while ((rc = libssh2_userauth_publickey_fromfile(session, username,

			"/home/user/"
			".ssh/id_rsa.pub",
			"/home/user/"
			".ssh/id_rsa",
			password)) ==
			LIBSSH2_ERROR_EAGAIN);
		if (rc) {
		}
	}

#if 0
	libssh2_trace(session, ~0);
#endif

	while ((channel = libssh2_channel_open_session(session)) == NULL &&
		libssh2_session_last_error(session, NULL, NULL, 0) == LIBSSH2_ERROR_EAGAIN)
	{
		waitsocket(sock, session);
	}
	if (channel == NULL)
	{
		exit(1);
	}
	else {
		g_disText << "channel -> libssh2_channel_open_session";
	}
}

QWorker_ssh::~QWorker_ssh()
{
	exitcode = 127;
	while ((rc = libssh2_channel_close(channel)) == LIBSSH2_ERROR_EAGAIN)
		waitsocket(sock, session);

	if (rc == 0)
	{
		exitcode = libssh2_channel_get_exit_status(channel);
		libssh2_channel_get_exit_signal(channel, &exitsignal,
			NULL, NULL, NULL, NULL, NULL);
	}

	if (exitsignal)
		fprintf(stderr, "\nGot signal: %s\n", exitsignal);
	else
		fprintf(stderr, "\nEXIT: %d bytecount: %d\n", exitcode, bytecount);

	libssh2_channel_free(channel);
	channel = NULL;

	libssh2_session_disconnect(session,	"Normal Shutdown, Thank you for playing");
	libssh2_session_free(session);


#ifdef WIN32
	closesocket(sock);
#else
	close(sock);
#endif
	fprintf(stderr, "all done\n");

	libssh2_exit();
}

int QWorker_ssh::waitsocket(int socket_fd, LIBSSH2_SESSION * session)
{
	struct timeval timeout;
	int rc;
	fd_set fd;
	fd_set *writefd = NULL;
	fd_set *readfd = NULL;
	int dir;

	timeout.tv_sec = 10;
	timeout.tv_usec = 0;

	FD_ZERO(&fd);

	FD_SET(socket_fd, &fd);

	/* now make sure we wait in the correct direction */
	dir = libssh2_session_block_directions(session);


	if (dir & LIBSSH2_SESSION_BLOCK_INBOUND)
		readfd = &fd;

	if (dir & LIBSSH2_SESSION_BLOCK_OUTBOUND)
		writefd = &fd;

	rc = select(socket_fd + 1, readfd, writefd, NULL, &timeout);

	return rc;
}

void QWorker_ssh::Exec(QString cmd)
{
	QByteArray ba = cmd.toLatin1();
	char *commandline = ba.data();

	if (channel == NULL)
	{
		emit sshRemoteResponse("ssh error : Exec channel NULL , check network and then reboot ssh tool !"); 
		return;
	}
	while ((rc = libssh2_channel_exec(channel, commandline)) ==	LIBSSH2_ERROR_EAGAIN)
	{
		waitsocket(sock, session);
	}
	if (rc != 0)
	{
		emit sshRemoteResponse("ssh error :libssh2_channel_exec , check network and then reboot ssh tool !");
		return;
	}
	for (;; )
	{
		/* loop until we block */
		int rc;
		char buffer[0x4000] = {0}; //16k
		bytecount = 0;
		do
		{
			rc = libssh2_channel_read(channel, &buffer[bytecount], sizeof(buffer));
			if (rc > 0)
			{
				int i;
				bytecount += rc;
				for (i = 0; i < rc; ++i)
					fputc(buffer[i], stderr);
			}
			else {
				if (rc != LIBSSH2_ERROR_EAGAIN)
					/* no need to output this for the EAGAIN case */
					fprintf(stderr, "libssh2_channel_read returned %d\n", rc);
			}
		} while (rc > 0);

		/* this is due to blocking that would occur otherwise so we loop on
		this condition */
		if (rc == LIBSSH2_ERROR_EAGAIN)
		{
			waitsocket(sock, session);
		}
		else
			break;
	}
}

