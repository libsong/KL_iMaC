#include "FiuNv.h"

extern QList<QString> g_disText;

macFiuNv::macFiuNv(QGroupBox *parent)
	: QGroupBox(parent)
{
	this->setTitle(weChinese2LocalCode("���������"));

	/*QGroupBox *act = new QGroupBox;*/
	QGroupBox *relay_grp = new QGroupBox;
	QGroupBox *mag = new QGroupBox(weChinese2LocalCode("˫��IP����Ԫ����޸��豸��Ϣ"));

	//�豸�������
	activeButt = new QPushButton(weChinese2LocalCode("����Զ���豸"));
	activeButt->setToolTip(weChinese2LocalCode("��λLED������ 1 ���Ϩ��˵���������ɹ�"));
	activeButt->setEnabled(true);
	activeButt->setFixedWidth(150);
	connect(activeButt, SIGNAL(clicked()), this, SLOT(deviceSFRST()));

	//
	relaytableButt = new QPushButton(weChinese2LocalCode("��������"));
	relaytableButt->setEnabled(true);
	relaytableButt->setFixedWidth(150);
	connect(relaytableButt, SIGNAL(clicked()), this, SLOT(relayactive()));

	cleartableButt = new QCheckBox(weChinese2LocalCode("���ȡ��ѡ������Relay"));
	cleartableButt->setChecked(true);
	connect(cleartableButt, SIGNAL(stateChanged(int)), this, SLOT(relayclear(int)));

	relaytable = new QTableWidget;
	relaytable->setColumnCount(8);
	relaytable->setRowCount(8);
	relaytable->setShowGrid(true);
	QHeaderView *head = relaytable->verticalHeader();
	//head->setHidden(true);
	//head = relaytable->horizontalHeader();
	//head->setHidden(true);
	//relaytable->horizontalHeader()->setStretchLastSection(false);
	//relaytable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	head = relaytable->horizontalHeader();
	head->setSectionResizeMode(QHeaderView::Stretch);

	QStringList lst;
	int relaytable_row = 1;
	for (int i = 0; i < 8; i++)
	{
		char buf[128];
		memset(buf, 0, 128);
		sprintf(buf, "Relay %d ~ %d", relaytable_row, relaytable_row + 7);
		lst << buf;

		relaytable_row += 8;
	}
	relaytable->setVerticalHeaderLabels(lst);

	for (int i = 0; i < relaytable->rowCount(); i++)
	{
		for (int j = 0; j < relaytable->columnCount(); j++) {
			//relaytable->horizontalHeader()->setSectionResizeMode(j, QHeaderView::ResizeToContents);
			relaytable->setEditTriggers(QAbstractItemView::NoEditTriggers);//��񲻿ɱ༭
																		   //relaytable->setColumnWidth(j, relaytable->size().width() / 1.8);
			QTableWidgetItem *check = new QTableWidgetItem;
			check->setCheckState(Qt::Checked);
			relaytable->setItem(i, j, check); //���븴ѡ��
		}
	}

	//QVBoxLayout *relaylayout = new QVBoxLayout;
	//relaylayout->addWidget(cleartableButt);
	//relaylayout->addWidget(relaytableButt);
	//relaylayout->addWidget(relaytable);

	QHBoxLayout *relayHlayout = new QHBoxLayout;
	relayHlayout->addWidget(cleartableButt);
	relayHlayout->addWidget(activeButt);	

	QVBoxLayout *relayVlayout = new QVBoxLayout;
	relayVlayout->addLayout(relayHlayout);
	relayVlayout->addWidget(relaytableButt);
	relayVlayout->addWidget(relaytable);

	relay_grp->setLayout(relayVlayout);

	//
	mcutable = new QTableWidget;//������ https://www.cnblogs.com/zhoug2020/p/3789076.html
	mcutable->setColumnCount(2);
	mcutable->setShowGrid(false);
	head = mcutable->verticalHeader();
	head->setHidden(true);
	head = mcutable->horizontalHeader();
	head->setHidden(true);
	mcutable->horizontalHeader()->setStretchLastSection(true);
	mcutable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	connect(mcutable, SIGNAL(cellChanged(int, int)), this, SLOT(ipChange(int, int)));

	QHBoxLayout *tle = new QHBoxLayout;
	tle->addWidget(mcutable);
	mag->setLayout(tle);

	vbox = new QVBoxLayout;
	//vbox->addWidget(act);
	//vbox->addWidget(relay_grp); //fiu �е����Ŀ������
	vbox->addWidget(mag);

	setLayout(vbox);
}

macFiuNv::~macFiuNv()
{

}

void macFiuNv::mcuInfoTable()
{
	isUserModifyCellSignal = false;

	char buf[128] = { 0 };

	mcutable->setRowCount(MCUINFOSTRUCT_ROW);

	mcutable->setItem(0, 0, new QTableWidgetItem("Device Name"));
	std::string str = mcu.devName;
	QString tmp = QString::fromStdString(str);
	mcutable->setItem(0, 1, new QTableWidgetItem(tmp));

	mcutable->setItem(1, 0, new QTableWidgetItem("Device Type"));
	memset(buf, 0, 128);
	sprintf(buf, "0x%x", mcu.devType);
	str = buf;
	tmp = QString::fromStdString(str);
	mcutable->setItem(1, 1, new QTableWidgetItem(tmp));

	mcutable->setItem(2, 0, new QTableWidgetItem("Device SN"));
	str = mcu.devSN;
	tmp = QString::fromStdString(str);
	mcutable->setItem(2, 1, new QTableWidgetItem(tmp));

	mcutable->setItem(3, 0, new QTableWidgetItem("IP Address"));
	memset(buf, 0, 128);
	sprintf(buf, "%d.%d.%d.%d", mcu.mcuIP[0], mcu.mcuIP[1], mcu.mcuIP[2], mcu.mcuIP[3]);
	str = buf;
	tmp = QString::fromStdString(str);
	mcutable->setItem(3, 1, new QTableWidgetItem(tmp));

	mcutable->setItem(4, 0, new QTableWidgetItem("IP Gateway"));
	memset(buf, 0, 128);
	sprintf(buf, "%d.%d.%d.%d", mcu.mcuGW[0], mcu.mcuGW[1], mcu.mcuGW[2], mcu.mcuGW[3]);
	str = buf;
	tmp = QString::fromStdString(str);
	mcutable->setItem(4, 1, new QTableWidgetItem(tmp));

	mcutable->setItem(5, 0, new QTableWidgetItem("IP Port"));
	memset(buf, 0, 128);
	sprintf(buf, "%d", mcu.port);
	str = buf;
	tmp = QString::fromStdString(str);
	mcutable->setItem(5, 1, new QTableWidgetItem(tmp));

	mcutable->setItem(6, 0, new QTableWidgetItem("IP Netmask"));
	memset(buf, 0, 128);
	sprintf(buf, "%d.%d.%d.%d", mcu.mcuNetMask[0], mcu.mcuNetMask[1], mcu.mcuNetMask[2], mcu.mcuNetMask[3]);
	str = buf;
	tmp = QString::fromStdString(str);
	mcutable->setItem(6, 1, new QTableWidgetItem(tmp));

	mcutable->setItem(7, 0, new QTableWidgetItem("Mcu NetMAC"));
	memset(buf, 0, 128);
	sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x", mcu.mcuMAC[0], mcu.mcuMAC[1], \
		mcu.mcuMAC[2], mcu.mcuMAC[3], mcu.mcuMAC[4], mcu.mcuMAC[5], mcu.mcuMAC[6]);
	str = buf;
	tmp = QString::fromStdString(str);
	mcutable->setItem(7, 1, new QTableWidgetItem(tmp));

	mcutable->setItem(8, 0, new QTableWidgetItem("Mcu Type"));
	str = mcu.mcuType;
	tmp = QString::fromStdString(str);
	mcutable->setItem(8, 1, new QTableWidgetItem(tmp));

	mcutable->setItem(9, 0, new QTableWidgetItem("Mcu UID"));
	memset(buf, 0, 128);
	sprintf(buf, "%x - %x - %x", mcu.mcuUID[0], mcu.mcuUID[1], mcu.mcuUID[2]);
	str = buf;
	tmp = QString::fromStdString(str);
	mcutable->setItem(9, 1, new QTableWidgetItem(tmp));

	mcutable->setItem(10, 0, new QTableWidgetItem("Sofeware Version"));
	memset(buf, 0, 128);
	sprintf(buf, "%d.%d.%d", mcu.mcuSV[0], mcu.mcuSV[1], mcu.mcuSV[2]);
	str = buf;
	tmp = QString::fromStdString(str);
	mcutable->setItem(10, 1, new QTableWidgetItem(tmp));

	mcutable->setItem(11, 0, new QTableWidgetItem("Motherboard Version"));
	memset(buf, 0, 128);
	sprintf(buf, "%d.%d.%d", mcu.mcuHV[0], mcu.mcuHV[1], mcu.mcuHV[2]);
	str = buf;
	tmp = QString::fromStdString(str);
	mcutable->setItem(11, 1, new QTableWidgetItem(tmp));

	mcutable->setItem(12, 0, new QTableWidgetItem("Reserve"));
	str = (char *)mcu.reserve;
	tmp = QString::fromStdString(str);
	mcutable->setItem(12, 1, new QTableWidgetItem(tmp));

	for (int i = 0; i < MCUINFOSTRUCT_ROW; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			if ((i == 3 || i == 4 || i == 5 || i == 6) && (j == 1))
			{
			}
			else
			{
				mcutable->item(i, j)->setFlags(mcutable->item(i, j)->flags() & ~Qt::ItemIsEditable);//���ɱ༭
				mcutable->item(i, j)->setFlags(mcutable->item(i, j)->flags() & ~Qt::ItemIsSelectable);//����ѡ��
			}
		}
	}

	isUserModifyCellSignal = true;
}

bool macFiuNv::IpCheck(QString ip)
{
	int tmp = -1;
	QList<QString> lst;
	lst = ip.split('.', QString::SkipEmptyParts);
	if (lst.size() != 4)
	{
		return false;
	}
	for (int i = 0; i < 4; i++) {
		tmp = lst[i].toInt(nullptr, 10);
		if (tmp < 0 || tmp > 255)
		{
			return false;
		}
	}
	return true;
}

void macFiuNv::SetIpChange()
{
	u8 crc = 0;
	qint64 rv = 0;
	QByteArray relay_data;
	relay_data.resize(53);
	for (int i = 0; i < 8; i++)
	{
		relay_data[i] = 0xbe;
	}

	relay_data[8] = COMM_CMD_IP;
	relay_data[9] = 0x0e;//len

	for (int i = 0; i < 4; i++)
	{
		relay_data[10 + i] = nIP[i];
		crc += nIP[i];
	}
	for (int i = 0; i < 2; i++)
	{
		relay_data[14 + i] = nPort[i];
		crc += nPort[i];
	}
	for (int i = 0; i < 4; i++)
	{
		relay_data[16 + i] = nNm[i];
		crc += nNm[i];
	}
	for (int i = 0; i < 4; i++)
	{
		relay_data[20 + i] = nGw[i];
		crc += nGw[i];
	}

	relay_data[24] = crc;//crc

	for (int i = 25; i < 33; i++)
	{
		relay_data[i] = 0xff;
	}

	//�豸mcuȥ�ж��Ƿ����UIDƥ�䣬ƥ������
	u8 tmp[12] = { 0 };
	memcpy(&tmp,&mcu.mcuUID[0],12);
	for (int i = 33; i < 45; i++)
	{
		relay_data[i] = tmp[i-33];
	}
	
	for (int i = 45; i < 53; i++)
	{
		relay_data[i] = 0xed;
	}

	QUdpSocket *uMulSocket = new QUdpSocket;
	if (uMulSocket->bind(QHostAddress::AnyIPv4, UDPMULCASTSEND_PORT, QUdpSocket::ShareAddress))
	{
		uMulSocket->setSocketOption(QAbstractSocket::MulticastLoopbackOption, 0);//��ֹ��������
		uMulSocket->joinMulticastGroup(QHostAddress(UDPMULCASTSEND_IP));
		uMulSocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 1024 * 1024 * 1);//���û�����
	}
	else
	{
		QMessageBox::information(NULL, "Warning", weChinese2LocalCode("��Ϣ�޸� �����ض˿ڰ�ʧ�ܣ�"), QMessageBox::Yes, QMessageBox::Yes);
		return;
	}

	rv = uMulSocket->writeDatagram(relay_data, relay_data.length(), QHostAddress(UDPMULCASTSEND_IP), UDPMULCASTSEND_PORT);
	if (rv != relay_data.length())
	{
		QMessageBox::information(NULL, "Warning", "��Ϣ�޸� ��дʧ�ܣ�", QMessageBox::Yes, QMessageBox::Yes);
	}
	else
	{
		g_disText << weChinese2LocalCode("Fiu �豸������Ϣ�޸���� .");
	}

	uMulSocket->leaveMulticastGroup(QHostAddress(UDPMULCASTSEND_IP));
	uMulSocket->disconnect();
	delete uMulSocket;

	/*u16 Port = mcu.port;
	char IP[32] = { 0 };
	sprintf(IP, "%d.%d.%d.%d", mcu.mcuIP[0], mcu.mcuIP[1], mcu.mcuIP[2], mcu.mcuIP[3]);

	QUdpSocket *uSocket = new QUdpSocket();
	rv = uSocket->writeDatagram(relay_data, relay_data.length(), QHostAddress(IP), Port);
	if (rv != relay_data.length())
	{
		QMessageBox::information(NULL, "Warning", "Informatin modify Failed !", QMessageBox::Yes, QMessageBox::Yes);
	}

	uSocket->disconnect();
	delete uSocket;*/
}

void macFiuNv::ipChange(int row, int col)
{
	if (isUserModifyCellSignal)
	{
		if (row == 3 && col == 1)
		{
			qDebug() << "ip modify";
		}
		if (row == 4 && col == 1)
		{
			qDebug() << "gw modify";
		}
		if (row == 5 && col == 1)
		{
			qDebug() << "port modify";
		}
		if (row == 6 && col == 1)
		{
			qDebug() << "nm modify";
		}

		QTableWidgetItem *item;
		QList<QString> lst;
		QString ip;
		item = mcutable->item(3, 1);
		ip = item->text();
		if (!IpCheck(ip))
		{
			QMessageBox::information(NULL, "Warning", weChinese2LocalCode("IP��Ϣ��ʽ����"), QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		lst = ip.split('.', QString::SkipEmptyParts);
		for (int i = 0; i < 4; i++) {
			nIP[i] = lst[i].toInt(nullptr, 10) & 0xff;
		}

		item = mcutable->item(4, 1);
		ip = item->text();
		if (!IpCheck(ip))
		{
			QMessageBox::information(NULL, "Warning", weChinese2LocalCode("IP��Ϣ��ʽ����"), QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		lst = ip.split('.', QString::SkipEmptyParts);
		for (int i = 0; i < 4; i++) {
			nGw[i] = lst[i].toInt(nullptr, 10) & 0xff;
		}

		item = mcutable->item(5, 1);
		QString Port = item->text();
		if (Port.toInt(nullptr, 10) < 0 || Port.toInt(nullptr, 10) > 65535)
		{
			QMessageBox::information(NULL, "Warning", weChinese2LocalCode("IP��Ϣ��ʽ����"), QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		nPort[0] = Port.toInt(nullptr, 10) & 0xff;
		nPort[1] = Port.toInt(nullptr, 10) >> 8;

		item = mcutable->item(6, 1);
		ip = item->text();
		if (!IpCheck(ip))
		{
			QMessageBox::information(NULL, "Warning", weChinese2LocalCode("IP��Ϣ��ʽ����"), QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		lst = ip.split('.', QString::SkipEmptyParts);
		for (int i = 0; i < 4; i++) {
			nNm[i] = lst[i].toInt(nullptr, 10) & 0xff;
		}

		SetIpChange();
		QMessageBox::information(NULL, "Warning", weChinese2LocalCode("IP����Ϣ������Ϊ��ֵ�������豸����Ч"), QMessageBox::Yes, QMessageBox::Yes);
	}
}

void macFiuNv::deviceSFRST()
{
	activeButt->setEnabled(false);//
	qint64 rv = 0;
	u8 crc = 0;
	QUdpSocket *uSocket = new QUdpSocket();

	QByteArray relay_data;
	relay_data.resize(27);
	for (int i = 0; i < 8; i++)
	{
		relay_data[i] = 0xbe;
	}

	relay_data[8] = COMM_CMD_SFRST;
	relay_data[9] = 0x0;//len

	relay_data[10] = 0;

	for (int i = 11; i < 19; i++)
	{
		relay_data[i] = 0xff;
	}
	for (int i = 19; i < 27; i++)
	{
		relay_data[i] = 0xed;
	}

	u16 Port = mcu.port;
	char IP[32] = { 0 };
	sprintf(IP, "%d.%d.%d.%d", mcu.mcuIP[0], mcu.mcuIP[1], mcu.mcuIP[2], mcu.mcuIP[3]);

	rv = uSocket->writeDatagram(relay_data, relay_data.length(), QHostAddress(IP), Port);
	qDebug() << "IP:" << IP << "port:" << Port << " writeDatagram : " << relay_data.toHex() << " rv:" << rv;
	if (rv != relay_data.length())
	{
		QMessageBox::information(NULL, "Warning", "Send Message Failed !", QMessageBox::Yes, QMessageBox::Yes);
	}
	else
	{
		g_disText << weChinese2LocalCode("FiuNv �豸��������У����Ժ� .");
	}

	uSocket->disconnect();
	delete uSocket;

	activeButt->setEnabled(true);
}

void macFiuNv::relayactive()
{
	relaytableButt->setEnabled(false);//
	qint64 rv = 0;
	u8 crc = 0;
	QUdpSocket *uSocket = new QUdpSocket();

	QByteArray relay_data;
	relay_data.resize(35);
	for (int i = 0; i < 8; i++)
	{
		relay_data[i] = 0xbe;
	}

	relay_data[8] = COMM_CMD_RELAY_FIU_ACT;
	relay_data[9] = 0x8;//len

	for (int j = 0; j < 8; j++)
	{
		relay_data[j + 10] = 0;

		for (int k = 0; k < 8; k++)
		{
			if (relaytable->item(j, k)->checkState() == Qt::Checked)
			{
				relay_data[j + 10] = (relay_data[j + 10] | (1 << k));
			}
		}

		crc += relay_data[j + 10];
	}

	relay_data[18] = crc;

	for (int i = 19; i < 27; i++)
	{
		relay_data[i] = 0xff;
	}
	for (int i = 27; i < 35; i++)
	{
		relay_data[i] = 0xed;
	}

	u16 Port = mcu.port;
	char IP[32] = { 0 };
	sprintf(IP, "%d.%d.%d.%d", mcu.mcuIP[0], mcu.mcuIP[1], mcu.mcuIP[2], mcu.mcuIP[3]);

	rv = uSocket->writeDatagram(relay_data, relay_data.length(), QHostAddress(IP), Port);
	qDebug() << "IP:" << IP << "port:" << Port << " writeDatagram : " << relay_data.toHex() << " rv:" << rv;
	if (rv != relay_data.length())
	{
		//QMessageBox::information(NULL, "Warning", "Send Message Failed !", QMessageBox::Yes, QMessageBox::Yes);
	}
	else
	{
		g_disText << weChinese2LocalCode("FiuNv �̵������ü���ɹ�");
	}

	uSocket->disconnect();
	delete uSocket;

	relaytableButt->setEnabled(true);
}

void macFiuNv::relayclear(int check)
{
	qDebug() << "cleartableButt check change";
	if (check == Qt::Checked)
	{
		cleartableButt->setText(weChinese2LocalCode("���ȡ��ѡ������Relay"));
		for (int i = 0; i < relaytable->rowCount(); i++)
		{
			for (int j = 0; j < relaytable->columnCount(); j++) {
				relaytable->item(i, j)->setCheckState(Qt::Checked);
			}
		}
	}
	else
	{
		cleartableButt->setText(weChinese2LocalCode("���ѡ������Relay"));
		for (int i = 0; i < relaytable->rowCount(); i++)
		{
			for (int j = 0; j < relaytable->columnCount(); j++) {
				relaytable->item(i, j)->setCheckState(Qt::Unchecked);
			}
		}
	}
}