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
#include "common.h"
#include "we_types.h"

//https://blog.csdn.net/polokang/article/details/6696982

//
//��Դ������ 
//
//

class macPowerSupplyBox : public QGroupBox
{
	Q_OBJECT

public:
	macPowerSupplyBox(QGroupBox *parent = 0);
	~macPowerSupplyBox();

	McuInfo_t	mcu;
	void mcuInfoTable();
	bool IpCheck(QString ip);
	void SetIpChange();

signals:
	

public slots:
	void relayactive();
	void ipChange(int row, int col);

private:
	QCheckBox		*radio1;
	QCheckBox		*radio2;
	QCheckBox		*radio3;
	QCheckBox		*radio4;
	QPushButton		*activeButt;

	QTableWidget	*mcutable;
	bool			isUserModifyCellSignal = true;//��ʶ ��Ϣ���/mcu�鲥��Ϣ�仯������ �����û�����ip�ȷ����� ��Ԫ�����ݸı��ź�
	u8				nIP[4];
	u8				nGw[4];
	u8				nPort[2];
	u8				nNm[4];

	QVBoxLayout		*vbox;
	QHBoxLayout		*hLayout;
};
