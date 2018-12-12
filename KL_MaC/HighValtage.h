#pragma once

#include <QGroupBox>
#include <QTableWidget>
#include <QCheckBox>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QUdpSocket>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QThread>
#include <QLineEdit>

#include <Winsock2.h>
#include <Windows.h>

#include "common.h"
#include "we_types.h"
#include "udp_mulcast.h"

//https://blog.csdn.net/polokang/article/details/6696982

//
//高压模拟器 
//
//

class queryThread : public QThread
{
	Q_OBJECT

public:
	QUdpSocket *uSocket;

	QString		Th_ip;
	u16			port;

	u8Tofloat	v[5];//实际电压流值
	bool		startRead = false;
signals:
	void giveQueryedVal(int type,void *val);
	
public slots:

private:
	void run();

};

class macHighValtage : public QGroupBox
{	Q_OBJECT

public:
	macHighValtage(QGroupBox *parent = 0);
	~macHighValtage();

	McuInfo_t	mcu;
	void mcuInfoTable();
	bool IpCheck(QString ip);
	void SetIpChange();

signals:
	void giveQueryIp(u8 *ip,u16 p);

public slots:
	void EmitQuerySig();
	void setVal(int type,void *);

	void relayactive();
	void relayclear(int check);
	void ipChange(int row, int col);

	void ResZAct();
	void ResFAct();

private:
	QLabel			*v1G;
	QLabel			*v1G_val;
	QLabel			*v12;
	QLabel			*v12_val;
	QLabel			*v34;
	QLabel			*v34_val;
	QLabel			*v56;
	QLabel			*v56_val;
	QLabel			*hall;
	QLabel			*hall_val;
	QPushButton		*activeButt;

	QPushButton		*m_actResZ;
	QPushButton		*m_actResF;
	QLineEdit		*m_ResZ;
	QLineEdit		*m_ResF;	

	QTableWidget	*relaytable;
	QPushButton		*relaytableButt;
	QCheckBox		*cleartableButt;

	QTableWidget	*mcutable;
	bool			isUserModifyCellSignal = true;//标识 信息填充/mcu组播信息变化发出的 还是用户更改ip等发出的 单元格内容改变信号
	u8				nIP[4];
	u8				nGw[4];
	u8				nPort[2];
	u8				nNm[4];

	QVBoxLayout		*vbox;
	QHBoxLayout		*hLayout;

	queryThread     *queryTh;
};



