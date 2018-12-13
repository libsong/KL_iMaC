#include "tree_sys.h"
#include <QTextCodec>
#include <windows.h>
#include <QDebug>
#include <QHostInfo>
#include <QNetworkInterface> 
#include <QList>
#include <QMessageBox>

macTreeSys::macTreeSys(QTreeWidget *parent)
	: QTreeWidget(parent)
{
	//setFixedHeight(500);
	setMinimumHeight(500);
	setFixedWidth(150);
	setStyleSheet("background-color:rgb(80,194,194);");
	setHeaderHidden(true);
	setContextMenuPolicy(Qt::CustomContextMenu);//右键响应

	setFocusPolicy(Qt::NoFocus);

	QTextCodec *codec = QTextCodec::codecForLocale();
	QString tmp;

	tmp = codec->toUnicode("本地系统");
	m_local = new QTreeWidgetItem(this, QStringList(tmp));
	m_local->setIcon(0, QIcon(":/KL_MaC/local_sys"));
		tmp = codec->toUnicode("本地信息");
		m_SysInfoLeaf = new QTreeWidgetItem(m_local, QStringList(tmp));
		m_SysInfoLeaf->setIcon(0, QIcon(":/KL_MaC/local_inf"));
		m_SysInfoLeaf->setToolTip(0, tmp);

	tmp = codec->toUnicode("远程系统");
	m_remote = new QTreeWidgetItem(this, QStringList(tmp));
	m_remote->setIcon(0, QIcon(":/KL_MaC/remote_sys"));
	remoteleaf_cnt = 0;

	connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)),\
		this, SLOT(showLeafLeftMouseClick(QTreeWidgetItem*, int)));

	this->expandAll();
}

macTreeSys::~macTreeSys()
{
	for (int i=0;i<REMOTELEAFMAX;i++)
	{
		if (m_leafinfo[i] != NULL)
		{
			delete m_leafinfo[i];
		}
		if (m_remoteleaf[i] != NULL)
		{
			delete m_remoteleaf[i];
		}
	}	
	
	delete m_NetInfoLeaf;
	delete m_SysInfoLeaf;
	delete m_remote;
	delete m_local;
}

void macTreeSys::GetLocalInfo(void)
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	qDebug() << "ProcessorType : " << sysinfo.dwProcessorType;
	qDebug() << "NumberOfProcessors : " << sysinfo.dwNumberOfProcessors;
	qDebug() << "PageSize : " << sysinfo.dwPageSize;
	qDebug() << "MaximumApplicationAddress : " << sysinfo.lpMaximumApplicationAddress;
	qDebug() << "MinimumApplicationAddress : " << sysinfo.lpMinimumApplicationAddress;

	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	qDebug() << "MemoryLoad : " << statex.dwMemoryLoad;
	qDebug() << "TotalPhys : " << statex.ullTotalPhys / MB;
	qDebug() << "AvailPhys : " << statex.ullAvailPhys / MB;
	qDebug() << "TotalVirtual : " << statex.ullTotalVirtual / MB;
	qDebug() << "AvailVirtual : " << statex.ullAvailVirtual / MB;

	QStringList name, val;
	name << weChinese2LocalCode("处理器类型") << weChinese2LocalCode("处理器数量") << weChinese2LocalCode("页大小") << weChinese2LocalCode("最大应用地址") << weChinese2LocalCode("最小应用地址")\
		<< weChinese2LocalCode("已用内存") << weChinese2LocalCode("总物理内存") << weChinese2LocalCode("可用物理内存") << weChinese2LocalCode("总虚拟内存") << weChinese2LocalCode("可用虚拟内存");
	val << QString::number(sysinfo.dwProcessorType, 10) << QString::number(sysinfo.dwNumberOfProcessors, 10)\
		<< QString::number(sysinfo.dwPageSize, 10) << "0x" + QString::number((int)sysinfo.lpMaximumApplicationAddress, 16).toUpper()\
		<< "0x" + QString::number((int)sysinfo.lpMinimumApplicationAddress, 16).toUpper()\
		<< QString::number(statex.dwMemoryLoad, 10) << QString::number(statex.ullTotalPhys / MB, 10)\
			<< QString::number(statex.ullAvailPhys / MB, 10) << QString::number(statex.ullTotalVirtual / MB, 10) << QString::number(statex.ullAvailVirtual / MB, 10);
	for (int i = 0; i < 10; i++)
	{
		emit GiveLocalInfo(0, i, 0, name.at(i));
		emit GiveLocalInfo(0, i, 1, val.at(i));
	}


	int			cnt_cnt = 0;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	UINT i;
	/* variables used to print DHCP time info */
	struct tm newtime;
	char buffer[32];
	errno_t we_error;

	//PIP_ADAPTER_INFO结构体存储本机网卡信息,包括本地网卡、无线网卡和虚拟网卡  
	PIP_ADAPTER_INFO pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	//调用GetAdaptersInfo函数,填充pAdapterInfo指针变量，其中ulOutBufLen参数既是输入也是输出  
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) != ERROR_SUCCESS)
	{
		//如果分配失败，释放后重新分配  
		//GlobalFree(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
	}
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR)
	{
		while (pAdapterInfo)
		{
			//pAdapter->Description中包含"PCI"为本地网卡，pAdapter->Type是71为无线网卡  
			if (strstr(pAdapterInfo->Description, "PCI") > 0 || pAdapterInfo->Type == 71)
			{
				
				//这里假设每个网卡只有一个IP  
				char * ipAddress = pAdapterInfo->IpAddressList.IpAddress.String;
				char tempIpAddress[3] = { '\0' };
				memcpy(tempIpAddress, ipAddress, 3);
				//只显示IP以192开头的网卡信息  
				//if (strstr(tempIpAddress, "192"))
				{
					qDebug("------------------------------------------------------------\n");
					qDebug("Adapter Name: \t%s\n", pAdapterInfo->AdapterName);
					qDebug("Adapter Description: \t%s\n", pAdapterInfo->Description);
					qDebug("Adapter Address: \t");
					for (UINT i = 0; i < pAdapterInfo->AddressLength; i++)
					{
						qDebug("%02X%c", pAdapterInfo->Address[i],
							i == pAdapterInfo->AddressLength - 1 ? '\n' : '-');
					}
					qDebug("Adapter Type: \t%d\n", pAdapterInfo->Type);
					qDebug("IP Address: \t%s\n", pAdapterInfo->IpAddressList.IpAddress.String);
					qDebug("IP Mask: \t%s\n", pAdapterInfo->IpAddressList.IpMask.String);

					char macbuf[64] = { 0 };
					sprintf(macbuf, "%02x:%02x:%02x:%02x:%02x:%02x", pAdapterInfo->Address[0], pAdapterInfo->Address[1], pAdapterInfo->Address[2],\
						pAdapterInfo->Address[3], pAdapterInfo->Address[4],pAdapterInfo->Address[5]);
					name << weChinese2LocalCode("网卡名称") << weChinese2LocalCode("网卡描述") << weChinese2LocalCode("MAC地址")\
						<< weChinese2LocalCode("网卡类型") << weChinese2LocalCode("IP 地址") << weChinese2LocalCode("IP 掩码");
					val << pAdapterInfo->AdapterName << pAdapterInfo->Description\
						<< macbuf << QString::number((int)pAdapterInfo->Type, 10)\
						<< pAdapterInfo->IpAddressList.IpAddress.String\
						<< pAdapterInfo->IpAddressList.IpMask.String ;
					for (int i = 10+cnt_cnt*6; i < 10 + cnt_cnt * 6 + 6; i++)
					{
						emit GiveLocalInfo(0, i, 0, name.at(i));
						emit GiveLocalInfo(0, i, 1, val.at(i));
					}
}
			}

			cnt_cnt++;
			pAdapterInfo = pAdapterInfo->Next;
		}
	}
	else
	{
		qDebug("Call to GetAdaptersInfo failed.\n");
	}
	if (pAdapterInfo)
	{
		GlobalFree(pAdapterInfo);
	}

#if 0
	//net
	QString tmp = qgetenv("USERNAME");
	qDebug() << "User Name : " << tmp;

	tmp = QHostInfo::localHostName();
	qDebug() << "LocalHostName : " << tmp;
	QHostInfo info = QHostInfo::fromName(tmp);
	for each (QHostAddress address in info.addresses())
	{
		if (address.protocol() == QAbstractSocket::IPv4Protocol)
		{
			qDebug() << "IPv4 : " << address.toString();
		}
	}

	qDebug() << "++++++++++++++++++++++++++++++++++";
	foreach(QNetworkInterface netInterface, QNetworkInterface::allInterfaces())
	{
		//netcard name 
		qDebug() << "Device:" << netInterface.name();
		//mac
		qDebug() << "Mac:" << netInterface.hardwareAddress();
		QList<QNetworkAddressEntry> entryList = netInterface.addressEntries();
		foreach(QNetworkAddressEntry entry, entryList)
		{
			//ip
			qDebug() << "Ip addr:" << entry.ip().toString();
			//mask
			qDebug() << "Netmask:" << entry.netmask().toString();
			//broad
			qDebug() << "Broadcast:" << entry.broadcast().toString();
		}
	}
#endif

	return;
}

void macTreeSys::showLeafRightMouseClick(const QPoint &pos)
{
	QTreeWidgetItem *curItem = this->itemAt(pos.x(),pos.y());
	if (curItem == NULL)//位置不在item范围内，空白区域
	{
		qDebug() << "right click at blank";
		return;
	}
	if (curItem != m_local && curItem != m_remote) {
		qDebug() << "right click";

		//TODO:添加叶子右键操作
	}
}

void macTreeSys::showLeafLeftMouseClick(QTreeWidgetItem* item, int col)
{
	QTreeWidgetItem *curItem = item;
	if (curItem == NULL)//位置不在item范围内，空白区域
	{
		qDebug() << "left click at blank";
		return;
	}

	QTreeWidgetItem *parent = item->parent();
	if (parent == m_local)
	{
		int col = parent->indexOfChild(item);
		qDebug() << "left click at col:"<< col;
		emit showStackWidgetLocal();
	}
	if (parent == m_remote) {
		/*int colcnt = m_local->columnCount();
		int col = parent->indexOfChild(item) + colcnt;
		qDebug() << "left click at col:" << col<<" colcnt:"<< colcnt;*/
		int col = parent->indexOfChild(item);
		qDebug() << "left click at col:" << col;
		emit showStackWidget(m_leafinfo[col]);//发送设备树的mcu信息，slot中根据信息的设备类型显示相应的widget
	}
}

void macTreeSys::MakeRemoteLeaf(void * val)
{
	bool uidExist = true;
	char name_tmp[128] = { 0 };

	McuInfo_t tmp;
	memset(&tmp, 0, sizeof(McuInfo_t));
	memcpy(&tmp,val,sizeof(McuInfo_t));

	if (remoteleaf_cnt == 0)
	{
		m_leafinfo[remoteleaf_cnt] = new McuInfo_t;
		memcpy(m_leafinfo[remoteleaf_cnt], &tmp, sizeof(McuInfo_t));

		QString uid0 = QString::number(tmp.mcuUID[0], 10);
		QString uid1 = QString::number(tmp.mcuUID[1], 10);
		QString uid2 = QString::number(tmp.mcuUID[2], 10);
		QString mapKey = uid0 + uid1 + uid2;
		mapRemoteLeaf.insert(std::map<QString, int>::value_type(mapKey,remoteleaf_cnt));

		if (tmp.devType == 0x0100)
		{
			memset(name_tmp, 0, 128);
			sprintf(name_tmp, "电源分配箱");
		}	
		if (tmp.devType == 0x0101)
		{
			memset(name_tmp, 0, 128);
			sprintf(name_tmp, "电源模拟箱");
		}
		if (tmp.devType == 0x0200)
		{
			memset(name_tmp, 0, 128);
			sprintf(name_tmp, "故障注入箱");
		}
		if (tmp.devType == 0x0300)
		{
			memset(name_tmp, 0, 128);
			sprintf(name_tmp, "高压模拟箱");
		}
		if (tmp.devType == 0x0400)
		{
			memset(name_tmp, 0, 128);
			sprintf(name_tmp, "信号调理箱");
		}
		if (tmp.devType == 0x0401)
		{
			memset(name_tmp, 0, 128);
			sprintf(name_tmp, "信号切换箱");
		}
		
		m_remoteleaf[remoteleaf_cnt] = new QTreeWidgetItem(m_remote, QStringList(weChinese2LocalCode(name_tmp)));
		m_remoteleaf[remoteleaf_cnt]->setIcon(0, QIcon(":/KL_MaC/net_on"));
		m_remoteleaf[remoteleaf_cnt]->setToolTip(0, weChinese2LocalCode(name_tmp) + weChinese2LocalCode(",双击可重命名"));
		m_remoteleaf[remoteleaf_cnt]->setFlags(m_remoteleaf[remoteleaf_cnt]->flags() | Qt::ItemIsEditable);

		remoteleaf_cnt++;
	}
	else {
		for (int i = 0; i < remoteleaf_cnt; i++)
		{
			if (m_leafinfo[i] != NULL) {				
				QString uid0 = QString::number(tmp.mcuUID[0], 10);
				QString uid1 = QString::number(tmp.mcuUID[1], 10);
				QString uid2 = QString::number(tmp.mcuUID[2], 10);
				QString mapKey = uid0 + uid1 + uid2;

				//tree 图标闪烁
				uid0 = QString::number(m_leafinfo[i]->mcuUID[0], 10);
				uid1 = QString::number(m_leafinfo[i]->mcuUID[1], 10);
				uid2 = QString::number(m_leafinfo[i]->mcuUID[2], 10);
				QString leafUid = uid0 + uid1 + uid2;
				if (leafUid == mapKey)
				{
					//网络连接闪烁图标
					if (m_bLeafIcon[i])
					{
						m_remoteleaf[i]->setIcon(0, QIcon(":/KL_MaC/net_on"));
					}
					else
					{
						m_remoteleaf[i]->setIcon(0, QIcon(":/KL_MaC/net_link"));
					}
					m_bLeafIcon[i] = !m_bLeafIcon[i];					
				}

				std::map<QString,int>::iterator key = mapRemoteLeaf.find(mapKey);
				if (key == mapRemoteLeaf.end())
				{
					uidExist = false;
					mapRemoteLeaf.insert(std::map<QString, int>::value_type(mapKey, remoteleaf_cnt));
				}

				if (!uidExist)//此设备不在树中 创建
				{
					uidExist = true;

					if (tmp.devType == 0x0100)
					{
						memset(name_tmp, 0, 128);
						sprintf(name_tmp, "电源分配箱");
					}
					if (tmp.devType == 0x0101)
					{
						memset(name_tmp, 0, 128);
						sprintf(name_tmp, "电源模拟箱");
					}
					if (tmp.devType == 0x0200)
					{
						memset(name_tmp, 0, 128);
						sprintf(name_tmp, "故障注入箱");
					}
					if (tmp.devType == 0x0300)
					{
						memset(name_tmp, 0, 128);
						sprintf(name_tmp, "高压模拟箱");
					}
					if (tmp.devType == 0x0400)
					{
						memset(name_tmp, 0, 128);
						sprintf(name_tmp, "信号调理箱");
					}
					if (tmp.devType == 0x0401)
					{
						memset(name_tmp, 0, 128);
						sprintf(name_tmp, "信号切换箱");
					}

					m_leafinfo[remoteleaf_cnt] = new McuInfo_t;
					memcpy(m_leafinfo[remoteleaf_cnt], &tmp, sizeof(McuInfo_t));

					m_remoteleaf[remoteleaf_cnt] = new QTreeWidgetItem(m_remote, QStringList(weChinese2LocalCode(name_tmp)));
					m_remoteleaf[remoteleaf_cnt]->setIcon(0, QIcon(":/KL_MaC/net_on"));
					m_remoteleaf[remoteleaf_cnt]->setToolTip(0, weChinese2LocalCode(name_tmp) + weChinese2LocalCode(",双击可重命名"));
					m_remoteleaf[remoteleaf_cnt]->setFlags(m_remoteleaf[remoteleaf_cnt]->flags() | Qt::ItemIsEditable);

					remoteleaf_cnt++;
					break;
				}
				else //设备树以创建 信息是否变化 如 ip 等
				{
					//从mapkey找到uid对应的cnt，再比较信息结构体
					QString uid0 = QString::number(tmp.mcuUID[0], 10);
					QString uid1 = QString::number(tmp.mcuUID[1], 10);
					QString uid2 = QString::number(tmp.mcuUID[2], 10);
					QString mapKey = uid0 + uid1 + uid2;
					int tmp_uidcnt = mapRemoteLeaf[mapKey];

					if (memcmp((u8 *)&tmp, (u8 *)m_leafinfo[tmp_uidcnt], sizeof(McuInfo_t)))
					{
						//更新信息
						*m_leafinfo[tmp_uidcnt] = tmp;
						emit showStackWidget(m_leafinfo[tmp_uidcnt]);//发送设备树的mcu信息，slot中根据信息的设备类型显示相应的widget
					}
				}
			}			
		}
	}
}

void macTreeSys::mouseReleaseEvent(QMouseEvent * event)
{
	if (event->button() & Qt::RightButton)
	{
		//emit customContextMenuRequested(event->pos());//使用信号slot中会有两次调用原因待查
		showLeafRightMouseClick(event->pos());
	}
	else if (event->button() & Qt::LeftButton)
	{
		emit itemClicked(itemFromIndex(indexAt(event->pos())), indexAt(event->pos()).column());
	}
	else
	{
		//event->ignore();
	}
}

QString weChinese2LocalCode(char *x)
{
	QTextCodec *codec = QTextCodec::codecForLocale();
	QString tmp;
	tmp = codec->toUnicode(x);

	return tmp;
}











