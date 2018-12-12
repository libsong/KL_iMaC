#include "PowerSimuBox.h"
#include <QDebug>

extern QList<QString> g_disText;

macPs::macPs(QGroupBox *parent)
	: QGroupBox(parent)
{
	this->setTitle(weChinese2LocalCode("电源模拟箱 - 控制与管理"));

	uSocket = new QUdpSocket();

	//cfg data
	//一共 38个字节 = 8个0xbe + 0x1 + 0xb + 11个配置字节（只用了前三个字节的 20 bit位）+ crc(11配置字节和校验，一个字节） + 8个0xff + 8个0xed
	m_relayDataCfg.resize(38);
	for (int i = 0; i < 8; i++)
	{
		m_relayDataCfg[i] = 0xbe;
	}
	m_relayDataCfg[8] = COMM_CMD_RELAY_CONF;
	m_relayDataCfg[9] = 0xb;//len
	for (int i = 22; i < 30; i++)
	{
		m_relayDataCfg[i] = 0xff;
	}
	for (int i = 30; i < 38; i++)
	{
		m_relayDataCfg[i] = 0xed;
	}

	//act data 固定 28个字节 = 8个0xbe + 0x9 + 0x1 + 0x1 + 0x1 + 8个0xff + 8个0xed
	m_relayDataAct.resize(28);
	for (int i = 0; i < 8; i++)
	{
		m_relayDataAct[i] = 0xbe;
	}
	m_relayDataAct[8] = COMM_CMD_PSB_ACT;
	m_relayDataAct[9] = 0x1;//len

	m_relayDataAct[10] = 1;
	m_relayDataAct[11] = 1;

	for (int i = 12; i < 20; i++)
	{
		m_relayDataAct[i] = 0xff;
	}
	for (int i = 20; i < 28; i++)
	{
		m_relayDataAct[i] = 0xed;
	}

	//
	m_unPwrVal = 0;

	//
	sp_tab = new QTabWidget;
	sp_tab->setTabPosition(QTabWidget::South);//tab标签位于下方
	sp_tab->setTabShape(QTabWidget::Triangular);//圆角

	QString tabBarStyle = "QTabBar::tab {min-width:100px;color: white;background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop: 0 #eeeeee, stop: 1 lightgray);\
								border: 1px solid;border-top-left-radius: 5px;border-top-right-radius: 5px;padding:2px;}\
								QTabBar::tab:!selected {margin-top: 3px;}QTabBar::tab:selected {color: blue;}"; 
	sp_tab->setStyleSheet(tabBarStyle);
												//
	mcutable = new QTableWidget;//表格操作 https://www.cnblogs.com/zhoug2020/p/3789076.html
	mcutable->setColumnCount(2);
	mcutable->setShowGrid(false);
	QHeaderView *head = mcutable->verticalHeader();
	head->setHidden(true);
	head = mcutable->horizontalHeader();
	head->setHidden(true);
	mcutable->horizontalHeader()->setStretchLastSection(true);
	mcutable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	connect(mcutable, SIGNAL(cellChanged(int, int)), this, SLOT(ipChange(int, int)));
	sp_tab->addTab(mcutable, weChinese2LocalCode("配置管理"));

	//设备控制
	m_pPwr1Grp = new QGroupBox(weChinese2LocalCode("电源 1"));
	m_pPwr1Grp->setFont(QFont("Microsoft Yahei", this->width() / 30));
	m_pPwr1Grp->setAlignment(Qt::AlignHCenter);
	m_pPwr2Grp = new QGroupBox(weChinese2LocalCode("电源 2"));
	m_pPwr2Grp->setFont(QFont("Microsoft Yahei", this->width() / 30));
	m_pPwr2Grp->setAlignment(Qt::AlignHCenter);
	m_pPwr = new QGroupBox();

	QHBoxLayout *hLayout = new QHBoxLayout;
	QHBoxLayout *hLayout2 = new QHBoxLayout;

	QString strOff, strOn;
	for (int i = 0; i < 10; i++)
	{
		m_pPwr1sw[i] = new SwitchButton(this);
		m_pPwr1sw[i]->setBgColor(QColor(253,113,113), QColor(92,159,20));
		m_pPwr1sw[i]->setIndex(i);
		strOff = "OFF" + QString::number(i+1, 10);
		strOn = "ON" + QString::number(i + 1, 10);
		m_pPwr1sw[i]->setText(strOff, strOn);
		m_pPwr1sw[i]->setMaximumSize(85, 25);
		connect(m_pPwr1sw[i], SIGNAL(checkedChanged(bool,int)), this, SLOT(onPwrBtnClicked(bool,int)));
		hLayout->addWidget(m_pPwr1sw[i]);

		m_pPwr2sw[i] = new SwitchButton(this);
		m_pPwr2sw[i]->setBgColor(QColor(253, 113, 113), QColor(92, 159, 20));
		m_pPwr2sw[i]->setIndex(i+10);
		strOff = "OFF" + QString::number(i + 11, 10);
		strOn = "ON" + QString::number(i + 11, 10);
		m_pPwr2sw[i]->setText(strOff, strOn);
		m_pPwr2sw[i]->setMaximumSize(85, 25);
		connect(m_pPwr2sw[i], SIGNAL(checkedChanged(bool, int)), this, SLOT(onPwrBtnClicked(bool, int)));
		hLayout2->addWidget(m_pPwr2sw[i]);
	}
	m_pPwr1Grp->setLayout(hLayout);
	m_pPwr2Grp->setLayout(hLayout2);

	//激活按钮
	activeButt = new QPushButton;
	activeButt->setMaximumWidth(this->width() / 16);
	activeButt->setMaximumHeight(this->width() / 16);
	activeButt->setToolTip(weChinese2LocalCode("点击激活"));
	activeButt->setStyleSheet("color:red");
	activeButt->setIcon(QIcon((":/KL_MaC/Resources/img/sysbutt/pwr_switch.png")));
	activeButt->setIconSize(QSize(this->width() / 16, this->width() / 16));
	activeButt->setFlat(true);
	connect(activeButt, SIGNAL(clicked(bool)),this,SLOT(actDeal(bool)));

	QVBoxLayout *vLayout = new QVBoxLayout;
	vLayout->addWidget(m_pPwr1Grp);
	vLayout->addWidget(m_pPwr2Grp);
	QHBoxLayout *hLayout3 = new QHBoxLayout;
	hLayout3->addStretch(this->width() - activeButt->width());
	hLayout3->addWidget(activeButt);
	vLayout->addLayout(hLayout3);

	m_pPwr->setLayout(vLayout);
	sp_tab->addTab(m_pPwr, weChinese2LocalCode("控制管理"));
	sp_tab->setCurrentIndex(1);

	//
	vbox = new QVBoxLayout;
	vbox->addWidget(sp_tab);

	setLayout(vbox);

	//
	uSocket = new QUdpSocket;
	connect(uSocket, SIGNAL(readyRead()), this, SLOT(receiveMul()));
}

macPs::~macPs()
{
	if (uSocket != nullptr)
	{
		uSocket->disconnect();
		delete uSocket;
	}
}

void macPs::mcuInfoTable()
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
				mcutable->item(i, j)->setFlags(mcutable->item(i, j)->flags() & ~Qt::ItemIsEditable);//不可编辑
				mcutable->item(i, j)->setFlags(mcutable->item(i, j)->flags() & ~Qt::ItemIsSelectable);//不可选择
			}
		}
	}

	isUserModifyCellSignal = true;
}

bool macPs::IpCheck(QString ip)
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

void macPs::SetIpChange()
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

	//设备mcu去判断是否和其UID匹配，匹配后更改
	u8 tmp[12] = { 0 };
	memcpy(&tmp, &mcu.mcuUID[0], 12);
	for (int i = 33; i < 45; i++)
	{
		relay_data[i] = tmp[i - 33];
	}

	for (int i = 45; i < 53; i++)
	{
		relay_data[i] = 0xed;
	}

	QUdpSocket *uMulSocket = new QUdpSocket;
	if (uMulSocket->bind(QHostAddress::AnyIPv4, UDPMULCASTSEND_PORT, QUdpSocket::ShareAddress))
	{
		uMulSocket->setSocketOption(QAbstractSocket::MulticastLoopbackOption, 0);//禁止本机接收
		uMulSocket->joinMulticastGroup(QHostAddress(UDPMULCASTSEND_IP));
		uMulSocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 1024 * 1024 * 1);//设置缓冲区
	}
	else
	{
		QMessageBox::information(NULL, "Warning", weChinese2LocalCode("信息修改 ：本地端口绑定失败！"), QMessageBox::Yes, QMessageBox::Yes);
		return;
	}

	rv = uMulSocket->writeDatagram(relay_data, relay_data.length(), QHostAddress(UDPMULCASTSEND_IP), UDPMULCASTSEND_PORT);
	if (rv != relay_data.length())
	{
		QMessageBox::information(NULL, "Warning", "信息修改 ：写失败！", QMessageBox::Yes, QMessageBox::Yes);
	}
	else
	{
		g_disText << weChinese2LocalCode("设备网络信息修改完成 .");
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

void macPs::ipChange(int row, int col)
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
			QMessageBox::information(NULL, "Warning", weChinese2LocalCode("IP信息格式错误"), QMessageBox::Yes, QMessageBox::Yes);
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
			QMessageBox::information(NULL, "Warning", weChinese2LocalCode("IP信息格式错误"), QMessageBox::Yes, QMessageBox::Yes);
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
			QMessageBox::information(NULL, "Warning", weChinese2LocalCode("IP信息格式错误"), QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		nPort[0] = Port.toInt(nullptr, 10) & 0xff;
		nPort[1] = Port.toInt(nullptr, 10) >> 8;

		item = mcutable->item(6, 1);
		ip = item->text();
		if (!IpCheck(ip))
		{
			QMessageBox::information(NULL, "Warning", weChinese2LocalCode("IP信息格式错误"), QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		lst = ip.split('.', QString::SkipEmptyParts);
		for (int i = 0; i < 4; i++) {
			nNm[i] = lst[i].toInt(nullptr, 10) & 0xff;
		}

		SetIpChange();
		QMessageBox::information(NULL, "Warning", weChinese2LocalCode("IP等信息已设置为新值，重启设备后生效"), QMessageBox::Yes, QMessageBox::Yes);
	}
}

void macPs::deviceSFRST()
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
		g_disText << weChinese2LocalCode("设备软件重启中，请稍后 .");
	}

	uSocket->disconnect();
	delete uSocket;

	activeButt->setEnabled(true);
}

void macPs::ClickedItem(QTreeWidgetItem *, int)
{
	//g_disText << "item click";
}

void macPs::actDeal(bool)
{
	activeButt->setEnabled(false);//

	qint64 rv = 0;
	
	u16 Port = mcu.port;
	char IP[32] = { 0 };
	sprintf(IP, "%d.%d.%d.%d", mcu.mcuIP[0], mcu.mcuIP[1], mcu.mcuIP[2], mcu.mcuIP[3]);

	if (uSocket == nullptr)
	{
		uSocket = new QUdpSocket;
		if (uSocket == nullptr)
		{
			g_disText << "Udp Create Failed !";
			activeButt->setEnabled(true);
			return;
		}
	}
	else
	{
		rv = uSocket->writeDatagram(m_relayDataCfg, m_relayDataCfg.length(), QHostAddress(IP), Port);
		if (rv != m_relayDataCfg.length())
		{
			g_disText << "Send Config Message Failed !";
			activeButt->setEnabled(true);
			return;
		}

		rv = uSocket->writeDatagram(m_relayDataAct, m_relayDataAct.length(), QHostAddress(IP), Port);
		if (rv != m_relayDataAct.length())
		{
			g_disText << "Send Active Message Failed !";
			activeButt->setEnabled(true);
			return;
		}
		else
		{
			QString tmp = "";
			for (int i = 0; i  < 20; i++)
			{
				if ((m_unPwrVal >> i) & 0x1)
				{
					tmp = tmp + "1";
				}
				else
				{
					tmp = tmp + "0";
				}
			}
			//g_disText << weChinese2LocalCode("电源模拟器 配置激活已发送") + " : " + QString("0b%1").arg(m_unPwrVal, 19, 2);
			g_disText << weChinese2LocalCode("电源模拟器 配置激活已发送") + " : " + tmp;
		}
	}

	activeButt->setEnabled(true);
}

void macPs::receiveMul()
{
	McuInfo_t mcu;
	bool err = false;
	u8 t;
	u8 crc = 0;

	if (uSocket != NULL)
	{
		while (uSocket->hasPendingDatagrams())
		{
			//qDebug() << "udp mulcast hasPendingDatagrams ...";

			quint32 datalen = uSocket->pendingDatagramSize();
			QByteArray datagram;
			datagram.resize(uSocket->pendingDatagramSize());
			uSocket->readDatagram(datagram.data(), datagram.size());
			//qDebug() << "Mulcast get len;" << datalen << ", data:" << datagram.toHex();

			// 进行处理的一些操作 8-be 8-aa 2-len 1-packettype len-() 1-crc 8-55 8-ed
			for (int i = 0; i < 8; i++)
			{
				t = datagram.data()[i];
				if (t != 0xbe)
				{
					err = true;
					goto WEERROR;
				}
			}
			for (int i = 8; i < 16; i++)
			{
				t = datagram.data()[i];
				if (t != 0xaa)
				{
					err = true;
					goto WEERROR;
				}
			}
			t = datagram.data()[17];
			u16 len = t << 8;
			t = datagram.data()[16];
			len = len | t;
			for (int i = 19; i < 19 + len; i++)
			{
				t = datagram.data()[i];
				crc += t;
			}
			t = datagram.data()[19 + len];
			if (crc != t)
			{
				err = true;
				goto WEERROR;
			}

			for (int i = 16 + 2 + 1 + len + 1; i < 16 + 2 + 1 + len + 1 + 8; i++)
			{
				t = datagram.data()[i];
				if (t != 0x55)
				{
					err = true;
					goto WEERROR;
				}
			}
			for (int i = 16 + 2 + 1 + len + 1 + 8; i < 16 + 2 + 1 + len + 1 + 8 + 8; i++)
			{
				t = datagram.data()[i];
				if (t != 0xed)
				{
					err = true;
					goto WEERROR;
				}
			}
			t = datagram.data()[18];
			if (!err && t == 0x01)//数据无错误并且是组播设备信息包
			{
				memcpy(&mcu, datagram.data() + 19, sizeof(McuInfo_t));
				//emit signalNewLeaf(&mcu);
				//emit signalNewWidget(&mcu);
			}

		WEERROR:
			err = false;
		}
	}
}

void macPs::onPwrBtnClicked(bool checked,int idx)
{
	if (checked)
	{
		m_unPwrVal |= 1<< idx;
	}
	else
	{
		m_unPwrVal &= ~(1 << idx);
	}

	u8 crc = 0;
	for (int j = 10; j < 21; j++)
	{
		m_relayDataCfg[j] = 0;

		if (j < 13)
		{
			m_relayDataCfg[j] = (m_unPwrVal >> (8 * (j - 10))) & 0xff;
		}

		crc += m_relayDataCfg[j];
	}

	m_relayDataCfg[21] = crc;
}

