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

	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);

	QStringList name, val;
	name << weChinese2LocalCode("处理器类型") << weChinese2LocalCode("处理器数量")
		<< weChinese2LocalCode("页大小") << weChinese2LocalCode("最大应用地址") 
		<< weChinese2LocalCode("最小应用地址") << weChinese2LocalCode("已用内存") 
		<< weChinese2LocalCode("总物理内存") << weChinese2LocalCode("可用物理内存") 
		<< weChinese2LocalCode("总虚拟内存") << weChinese2LocalCode("可用虚拟内存");
	val << QString::number(sysinfo.dwProcessorType, 10) << QString::number(sysinfo.dwNumberOfProcessors, 10)
		<< QString::number(sysinfo.dwPageSize, 10) << "0x" + QString::number((int)sysinfo.lpMaximumApplicationAddress, 16).toUpper()
		<< "0x" + QString::number((int)sysinfo.lpMinimumApplicationAddress, 16).toUpper()
		<< QString::number(statex.dwMemoryLoad, 10) << QString::number(statex.ullTotalPhys / MB, 10)
		<< QString::number(statex.ullAvailPhys / MB, 10) << QString::number(statex.ullTotalVirtual / MB, 10) 
		<< QString::number(statex.ullAvailVirtual / MB, 10);
	for (int i = 0; i < 10; i++)
	{
		emit GiveLocalInfo(0, i, 0, name.at(i));
		emit GiveLocalInfo(0, i, 1, val.at(i));
	}

	//PIP_ADAPTER_INFO结构体指针存储本机网卡信息
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	//得到结构体大小,用于GetAdaptersInfo参数
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	//调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量;其中stSize参数既是一个输入量也是一个输出量
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	//记录网卡数量
	int netCardNum = 0;
	//记录每张网卡上的IP地址数量
	int IPnumPerNetCard = 0;
	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		//如果函数返回的是ERROR_BUFFER_OVERFLOW
		//则说明GetAdaptersInfo参数传递的内存空间不够,同时其传出stSize,表示需要的空间大小
		//这也是说明为什么stSize既是一个输入量也是一个输出量
		//释放原来的内存空间
		delete pIpAdapterInfo;
		//重新申请内存空间用来存储所有网卡信息
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
		//再次调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	}
	if (ERROR_SUCCESS == nRel)
	{
		//输出网卡信息
		//可能有多网卡,因此通过循环去判断
		QString nicType = "";
		while (pIpAdapterInfo)
		{
			switch (pIpAdapterInfo->Type)
			{
			case MIB_IF_TYPE_OTHER:
				nicType = "OTHER";
				break;
			case MIB_IF_TYPE_ETHERNET:
				nicType = "ETHERNET";
				break;
			case MIB_IF_TYPE_TOKENRING:
				nicType = "TOKENRING";
				break;
			case MIB_IF_TYPE_FDDI:
				nicType = "FDDI";
				break;
			case MIB_IF_TYPE_PPP:
				printf("PP\n");
				nicType = "PPP";
				break;
			case MIB_IF_TYPE_LOOPBACK:
				nicType = "LOOPBACK";
				break;
			case MIB_IF_TYPE_SLIP:
				nicType = "SLIP";
				break;
			default:
				nicType = "Unknown";
				break;
			}
			for (DWORD i = 0; i < pIpAdapterInfo->AddressLength; i++)
				if (i < pIpAdapterInfo->AddressLength - 1)
				{
					printf("%02X-", pIpAdapterInfo->Address[i]);
				}
				else
				{
					printf("%02X\n", pIpAdapterInfo->Address[i]);
				}

			//可能网卡有多IP,因此通过循环去判断
			IP_ADDR_STRING *pIpAddrString = &(pIpAdapterInfo->IpAddressList);
			do
			{
				char macbuf[64] = { 0 };
				sprintf(macbuf, "%02x:%02x:%02x:%02x:%02x:%02x", pIpAdapterInfo->Address[0], pIpAdapterInfo->Address[1], pIpAdapterInfo->Address[2], \
					pIpAdapterInfo->Address[3], pIpAdapterInfo->Address[4], pIpAdapterInfo->Address[5]);
				name << weChinese2LocalCode("网卡名称") 
					<< weChinese2LocalCode("网卡描述") 
					<< weChinese2LocalCode("网卡MAC")
					<< weChinese2LocalCode("网卡类型") 
					<< weChinese2LocalCode("IP 地址") 
					<< weChinese2LocalCode("IP 掩码")
					<< weChinese2LocalCode("IP 网关");
				val << pIpAdapterInfo->AdapterName
					<< pIpAdapterInfo->Description
					<< macbuf 
					<< nicType
					<< pIpAdapterInfo->IpAddressList.IpAddress.String
					<< pIpAdapterInfo->IpAddressList.IpMask.String
					<< pIpAdapterInfo->GatewayList.IpAddress.String;
				for (int i = 10 + netCardNum * 7; i < 10 + netCardNum * 7 + 7; i++)
				{
					emit GiveLocalInfo(0, i, 0, name.at(i));
					emit GiveLocalInfo(0, i, 1, val.at(i));
				}

				pIpAddrString = pIpAddrString->Next;				
			} while (pIpAddrString);

			pIpAdapterInfo = pIpAdapterInfo->Next;
			netCardNum++;
#if 0
			cout << "网卡数量：" << ++netCardNum << endl;
			cout << "网卡名称：" << pIpAdapterInfo->AdapterName << endl;
			cout << "网卡描述：" << pIpAdapterInfo->Description << endl;
			switch (pIpAdapterInfo->Type)
			{
			case MIB_IF_TYPE_OTHER:
				cout << "网卡类型：" << "OTHER" << endl;
				break;
			case MIB_IF_TYPE_ETHERNET:
				cout << "网卡类型：" << "ETHERNET" << endl;
				break;
			case MIB_IF_TYPE_TOKENRING:
				cout << "网卡类型：" << "TOKENRING" << endl;
				break;
			case MIB_IF_TYPE_FDDI:
				cout << "网卡类型：" << "FDDI" << endl;
				break;
			case MIB_IF_TYPE_PPP:
				printf("PP\n");
				cout << "网卡类型：" << "PPP" << endl;
				break;
			case MIB_IF_TYPE_LOOPBACK:
				cout << "网卡类型：" << "LOOPBACK" << endl;
				break;
			case MIB_IF_TYPE_SLIP:
				cout << "网卡类型：" << "SLIP" << endl;
				break;
			default:

				break;
			}
			cout << "网卡MAC地址：";
			for (DWORD i = 0; i < pIpAdapterInfo->AddressLength; i++)
				if (i < pIpAdapterInfo->AddressLength - 1)
				{
					printf("%02X-", pIpAdapterInfo->Address[i]);
				}
				else
				{
					printf("%02X\n", pIpAdapterInfo->Address[i]);
				}
			cout << "网卡IP地址如下：" << endl;
			//可能网卡有多IP,因此通过循环去判断
			IP_ADDR_STRING *pIpAddrString = &(pIpAdapterInfo->IpAddressList);
			do
			{
				cout << "该网卡上的IP数量：" << ++IPnumPerNetCard << endl;
				cout << "IP 地址：" << pIpAddrString->IpAddress.String << endl;
				cout << "子网地址：" << pIpAddrString->IpMask.String << endl;
				cout << "网关地址：" << pIpAdapterInfo->GatewayList.IpAddress.String << endl;
				pIpAddrString = pIpAddrString->Next;
			} while (pIpAddrString);
			pIpAdapterInfo = pIpAdapterInfo->Next;
			cout << "--------------------------------------------------------------------" << endl;
#endif
		}
	}
	//释放内存空间
	if (pIpAdapterInfo)
	{
		delete pIpAdapterInfo;
	}
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











