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

#include "RepaintControl.h"

//https://blog.csdn.net/polokang/article/details/6696982

//
// 电源模拟箱 20 relays , PC181726
//
//

class macPsbtn : public QPushButton
{
	Q_OBJECT

public:
	macPsbtn(const QString &text, QWidget *parent, int r, int c);
	~macPsbtn();

signals:
	void webtnClickAt(int r, int c);

protected:
	void mouseReleaseEvent(QMouseEvent * e);

private:
	int column_at;
	int row_at;
};

class macPs : public QGroupBox
{
	Q_OBJECT

public:
	macPs(QGroupBox *parent = 0);
	~macPs();

	McuInfo_t	mcu;
	void mcuInfoTable();		//创建widget 或者 mcu信息变化时， 都会被调用一次，此时的mcu信息是最新的
	bool IpCheck(QString ip);
	void SetIpChange();

signals:
	void giveQueryIp(u8 *ip, u16 p);

public slots:
	void ipChange(int row, int col);
	void deviceSFRST();

	void ClickedItem(QTreeWidgetItem *, int);
	void receiveMul();
	void onPwrBtnClicked(bool checked, int idx);
	void actDeal(bool);

private:
	QTabWidget		*sp_tab;
	QTableWidget	*mcutable;
	macPsbtn		*slave_status_btn;
	macPsbtn		*slave_gain_btn;

	QGroupBox		*m_pPwr;
	QGroupBox		*m_pPwr1Grp;
	SwitchButton	*m_pPwr1sw[10];
	QGroupBox		*m_pPwr2Grp;
	SwitchButton	*m_pPwr2sw[10];
	quint32			m_unPwrVal;

	QPushButton		*activeButt;

	bool			isUserModifyCellSignal = true;//标识 信息填充/mcu组播信息变化发出的, 还是用户更改ip单元格内容改变等发出的信号
	u8				nIP[4];
	u8				nGw[4];
	u8				nPort[2];
	u8				nNm[4];

	QVBoxLayout		*vbox;
	QHBoxLayout		*hLayout;

	QUdpSocket		*uSocket = nullptr;
	QByteArray		m_relayDataCfg;
	QByteArray		m_relayDataAct;
};













