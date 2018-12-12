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
#include <QTabWidget>
#include <QTreewidget>
#include <QComboBox>
#include <QListView>
#include <Winsock2.h>
#include <Windows.h>
#include <QMouseEvent>

#include "common.h"
#include "we_types.h"
#include "udp_mulcast.h"

//https://blog.csdn.net/polokang/article/details/6696982

//
// 信号调理箱
//
//

#define MAXSLAVEBOARDNUM 18 //一个调理箱 18 块板
#define MAXSLAVECHANNELNUM 16	//每块板 16 通道

#define I2CHAR(x) #@x


class macSPcombox : public QComboBox
{
	Q_OBJECT

public:
	macSPcombox(QWidget *parent = Q_NULLPTR);
	~macSPcombox();

private:

};

class macSPbtn : public QPushButton
{
	Q_OBJECT

public:
	macSPbtn(const QString &text, QWidget *parent, int r, int c);
	~macSPbtn();

signals:
	void webtnClickAt(int r, int c);

protected:
	void mouseReleaseEvent(QMouseEvent * e);

private:
	int column_at;
	int row_at;
};

class macSP : public QGroupBox
{
	Q_OBJECT

public:
	macSP(QGroupBox *parent = 0);
	~macSP();

	McuInfo_t	mcu;
	void mcuInfoTable();		//创建widget 或者 mcu信息变化时， 都会被调用一次，此时的mcu信息是最新的
	bool IpCheck(QString ip);
	void SetIpChange();
	void queryCmdSend(qint8 cardNum);
	void gainSetCmdSend(qint8 cardNum);
	void formatGainString(int gainIdx,char *val);
	void analysisGainString(QByteArray data);

signals:
	void giveQueryIp(u8 *ip, u16 p);

public slots:
	void ipChange(int row, int col);
	void deviceSFRST();

	void ClickedItem(QTreeWidgetItem *,int);
	void webtnClickDeal(int r, int c);
	void receiveMul();

	void weTreeClick(const QModelIndex &idx);
	void weHeadClick(int head);

private:
	QTabWidget		*sp_tab;
	QTableWidget	*mcutable;
	QTreeWidget		*slaveTree;
	QTreeWidgetItem *slave[MAXSLAVEBOARDNUM];
	QTreeWidgetItem *slave_chl[MAXSLAVECHANNELNUM];
	macSPbtn		*slave_status_btn[MAXSLAVEBOARDNUM];
	macSPbtn		*slave_gain_btn[MAXSLAVEBOARDNUM];
	macSPcombox		*m_pGainComBox[MAXSLAVEBOARDNUM*MAXSLAVECHANNELNUM];
	QLabel			*m_pStatusLabel[MAXSLAVEBOARDNUM*MAXSLAVECHANNELNUM];
	QLabel			*m_pTypeLabel[MAXSLAVEBOARDNUM];

	QPushButton		*activeButt;

	bool			isUserModifyCellSignal = true;//标识 信息填充/mcu组播信息变化发出的, 还是用户更改ip单元格内容改变等发出的信号
	u8				nIP[4];
	u8				nGw[4];
	u8				nPort[2];
	u8				nNm[4];

	QVBoxLayout		*vbox;
	QHBoxLayout		*hLayout;

	QUdpSocket		*uSocket = nullptr;

	int				m_sHeadClickCnt = 0;
};













