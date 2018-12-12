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
// �ź��л��� 144 relays 
//
// ��ť�ֲ����� 
// �������� : �ź��л���ť
// �������һ�� ����ѡ��
// �������һ�� ����ѡ��
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

#define SWROW_NUM 19 // ��һ�е� �� ѡ�� + ��18 �е� relay ,ÿ�� 8 ����
#define SWCOLUMN_NUM	9 //��һ�е� �� ѡ�� + ��8 �� relay��

class macSw : public QGroupBox
{
	Q_OBJECT

public:
	macSw(QGroupBox *parent = 0);
	~macSw();

	McuInfo_t	mcu;
	void mcuInfoTable();		//����widget ���� mcu��Ϣ�仯ʱ�� ���ᱻ����һ�Σ���ʱ��mcu��Ϣ�����µ�
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

	bool			isUserModifyCellSignal = true;//��ʶ ��Ϣ���/mcu�鲥��Ϣ�仯������, �����û�����ip��Ԫ�����ݸı�ȷ������ź�
	u8				nIP[4];
	u8				nGw[4];
	u8				nPort[2];
	u8				nNm[4];

	QVBoxLayout		*vLayout;
	QHBoxLayout		*hLayout[SWROW_NUM];

	QUdpSocket		*uSocket = nullptr;
	QByteArray		m_relayDataCfg;
};













