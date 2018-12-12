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

//
// 信号切换箱 144 relays 
//
// 按钮分布如下 
// 虚线右下 : 信号切换按钮
// 虚线外第一行 ：列选择
// 虚线外第一列 ：行选择
// 		0		1	2	3	4	5	6	7	8
// 			-	-	-	-	-	-	-	-	-	-
// 		9	|	10	11	12	13	14	15	16	17
// 		18	|	19	20	21	22	23	24	25	26
// 		27	|
// 		36	|
// 		45	|
// 		54	|
// 		63	|
// 		72	|
// 		81	|
// 		90 	|
// 		99	|
// 		108	|
// 		117	|
// 		126	|
// 		135	|
// 		144	|
// 		153	|
// 		162	|

#define SWROW_NUM 19 // 第一行的 列 选中 + （18 行的 relay ,每行 8 个）
#define SWCOLUMN_NUM	9 //第一列的 行 选中 + （8 个 relay）

class macSw : public QGroupBox
{
	Q_OBJECT

public:
	macSw(QGroupBox *parent = 0);
	~macSw();

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

	QGroupBox		*m_pPwr;
	QGroupBox		*m_pPwr1Grp;
	SwitchButton	*m_pPwr1sw[SWCOLUMN_NUM*SWROW_NUM];

	QPushButton		*activeButt;
	weAnimationPushButton		*activeButt1;

	bool			isUserModifyCellSignal = true;//标识 信息填充/mcu组播信息变化发出的, 还是用户更改ip单元格内容改变等发出的信号
	u8				nIP[4];
	u8				nGw[4];
	u8				nPort[2];
	u8				nNm[4];

	QVBoxLayout		*vLayout;
	QHBoxLayout		*hLayout[SWROW_NUM];

	QUdpSocket		*uSocket = nullptr;
	QByteArray		m_relayDataCfg;
};













