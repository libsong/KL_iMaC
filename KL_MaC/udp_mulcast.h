#pragma once

#include <QUdpSocket>
#include <QThread>
#include <QObject>
#include <QMutex>
#include <QMessageBox>
#include <QNetworkConfigurationManager>
#include <QApplication>
#include <WinSock2.h>
#include <iphlpapi.h>

#include "common.h"
#include "we_types.h"

#define UDPMULCASTRECV_IP "225.226.227.228"		//接收来自mcu的组播包
#define UDPMULCASTRECV_PORT 54345			

#define UDPMULCASTSEND_IP "235.236.237.238"		//发送给mcu的组播包
#define UDPMULCASTSEND_PORT 45654

#if 1
class macUdpMulcast : public QThread
{
	Q_OBJECT

public:
	macUdpMulcast();
	~macUdpMulcast();

	void getAdapterInfoWithWindows();
	void macUdp_send(const QString &ip,int port, QByteArray *data);

signals:
	void signalNewLeaf(void *mcu);
	void signalNewWidget(void *mcu);

public slots:
	void receiveMul();

private:
	QUdpSocket *uMulSocket;

};

#endif










