#pragma once

#include <QtCore/QCoreApplication>
#include <QDebug>

#include "libssh2_config.h"
#include "libssh2.h"
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <sys/types.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

#include <winsock2.h>
#pragma comment(lib, "WS2_32")
#pragma comment(lib, "legacy_stdio_definitions.lib")

#include <QPlainTextEdit>  
#include <QScrollBar>

#include "we_types.h"

class QWorker_ssh : public QObject
{
	Q_OBJECT

public:
	explicit QWorker_ssh(QObject *parent,QString ip, QString usr, QString pwd);
	~QWorker_ssh();

	int waitsocket(int socket_fd, LIBSSH2_SESSION *session);

signals:
	void sshRemoteResponse(QString str);

public slots:
	void Exec(QString cmd);

private:
	QString r_ip;
	QString r_usr;
	QString r_pwd;


	char *hostname = "127.0.0.1";
	char *username = "user";
	char *password = "password";
	unsigned long hostaddr;
	int sock;
	struct sockaddr_in sin;
	const char *fingerprint;
	LIBSSH2_SESSION *session;
	LIBSSH2_CHANNEL *channel = NULL;
	int rc;
	int exitcode;
	char *exitsignal = (char *)"none";
	int bytecount = 0;
	size_t len;
	LIBSSH2_KNOWNHOSTS *nh;
	int type;
};


class QLineEdit;
class macShell :public QPlainTextEdit
{
	Q_OBJECT

public:
	explicit macShell(QWidget *parent = 0);
	~macShell();
	virtual QString runCommand(const QString& cmd);

	void saveSshObjInf(QString ip, QString usr, QString pwd);

signals:
	void giveRemoteInfo(QString ip, QString usr, QString pwd);
	void giveCmdLine(QString cmd);

public slots:
	void remoteResponse(QString str);

protected:
	void resizeEvent(QResizeEvent *e);
	private slots:
	void onScrollBarValueChanged();
	void onEditFinished();

private:
	void updateEditPosition();
	QLineEdit * edit;

	QString m_ip;
	QString m_usr ;
	QString m_pwd ;

	QThread		*m_pThread;
	QWorker_ssh *m_pWorker;
};