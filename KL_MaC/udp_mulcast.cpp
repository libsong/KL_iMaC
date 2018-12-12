#include "udp_mulcast.h"

#if 1
macUdpMulcast::macUdpMulcast()
{
	QNetworkConfigurationManager mgr;

	if (mgr.isOnline())
	{
		//uMulSocket
		uMulSocket = new QUdpSocket;
		if (uMulSocket->bind(QHostAddress::AnyIPv4, UDPMULCASTRECV_PORT, QUdpSocket::ShareAddress))
		{
			uMulSocket->setSocketOption(QAbstractSocket::MulticastLoopbackOption, 0);//禁止本机接收
			uMulSocket->joinMulticastGroup(QHostAddress(UDPMULCASTRECV_IP));
			uMulSocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 1024 * 1024 * 1);//设置缓冲区
			connect(uMulSocket, SIGNAL(readyRead()), this, SLOT(receiveMul()));
			qDebug() << "udp mulcast init done ...";
		}
	}
	else
	{
		QMessageBox::warning(NULL,"Net Check Failed !!!", weChinese2LocalCode("网络初始化失败，请检查网络后重启软件!"),NULL,NULL);
		//qApp->quit();
	}	
}

macUdpMulcast::~macUdpMulcast()
{
	
	if (uMulSocket != NULL)
	{
		uMulSocket->disconnect();
		delete uMulSocket;
	}
}

void macUdpMulcast::getAdapterInfoWithWindows()//https://blog.csdn.net/caoshangpa/article/details/51073138
{
	/* Declare and initialize variables */

	// It is possible for an adapter to have multiple
	// IPv4 addresses, gateways, and secondary WINS servers
	// assigned to the adapter. 
	//
	// Note that this sample code only prints out the 
	// first entry for the IP address/mask, and gateway, and
	// the primary and secondary WINS server for each adapter. 

	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	UINT i;

	/* variables used to print DHCP time info */
	struct tm newtime;
	char buffer[32];
	errno_t we_error;

	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL) {
		printf("Error allocating memory needed to call GetAdaptersinfo\n");
		return ;
	}
	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		GlobalFree(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
		if (pAdapterInfo == NULL) {
			printf("Error allocating memory needed to call GetAdaptersinfo\n");
			return ;
		}
	}

	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
		pAdapter = pAdapterInfo;
		while (pAdapter) {
			printf("\tComboIndex: \t%d\n", pAdapter->ComboIndex);
			printf("\tAdapter Name: \t%s\n", pAdapter->AdapterName);
			printf("\tAdapter Desc: \t%s\n", pAdapter->Description);
			printf("\tAdapter Addr: \t");
			for (i = 0; i < pAdapter->AddressLength; i++) {
				if (i == (pAdapter->AddressLength - 1))
					printf("%.2X\n", (int)pAdapter->Address[i]);
				else
					printf("%.2X-", (int)pAdapter->Address[i]);
			}
			printf("\tIndex: \t%d\n", pAdapter->Index);
			printf("\tType: \t");
			switch (pAdapter->Type) {
			case MIB_IF_TYPE_OTHER:
				printf("Other\n");
				break;
			case MIB_IF_TYPE_ETHERNET:
				printf("Ethernet\n");
				break;
			case MIB_IF_TYPE_TOKENRING:
				printf("Token Ring\n");
				break;
			case MIB_IF_TYPE_FDDI:
				printf("FDDI\n");
				break;
			case MIB_IF_TYPE_PPP:
				printf("PPP\n");
				break;
			case MIB_IF_TYPE_LOOPBACK:
				printf("Lookback\n");
				break;
			case MIB_IF_TYPE_SLIP:
				printf("Slip\n");
				break;
			default:
				printf("Unknown type %ld\n", pAdapter->Type);
				break;
			}

			printf("\tIP Address: \t%s\n",
				pAdapter->IpAddressList.IpAddress.String);
			printf("\tIP Mask: \t%s\n", pAdapter->IpAddressList.IpMask.String);

			printf("\tGateway: \t%s\n", pAdapter->GatewayList.IpAddress.String);
			printf("\t***\n");

			if (pAdapter->DhcpEnabled) {
				printf("\tDHCP Enabled: Yes\n");
				printf("\t  DHCP Server: \t%s\n",
					pAdapter->DhcpServer.IpAddress.String);

				printf("\t  Lease Obtained: ");
				/* Display local time */
				we_error = _localtime32_s(&newtime, (__time32_t*)&pAdapter->LeaseObtained);
				if (we_error)
					printf("Invalid Argument to _localtime32_s\n");
				else {
					// Convert to an ASCII representation 
					we_error = asctime_s(buffer, 32, &newtime);
					if (we_error)
						printf("Invalid Argument to asctime_s\n");
					else
						/* asctime_s returns the string terminated by \n\0 */
						printf("%s", buffer);
				}

				printf("\t  Lease Expires:  ");
				we_error = _localtime32_s(&newtime, (__time32_t*)&pAdapter->LeaseExpires);
				if (we_error)
					printf("Invalid Argument to _localtime32_s\n");
				else {
					// Convert to an ASCII representation 
					we_error = asctime_s(buffer, 32, &newtime);
					if (we_error)
						printf("Invalid Argument to asctime_s\n");
					else
						/* asctime_s returns the string terminated by \n\0 */
						printf("%s", buffer);
				}
			}
			else
				printf("\tDHCP Enabled: No\n");

			if (pAdapter->HaveWins) {
				printf("\tHave Wins: Yes\n");
				printf("\t  Primary Wins Server:    %s\n",
					pAdapter->PrimaryWinsServer.IpAddress.String);
				printf("\t  Secondary Wins Server:  %s\n",
					pAdapter->SecondaryWinsServer.IpAddress.String);
			}
			else
				printf("\tHave Wins: No\n");
			pAdapter = pAdapter->Next;
			printf("\n");
}
	}
	else {
		printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);

	}
	if (pAdapterInfo)
		GlobalFree(pAdapterInfo);


#if 0
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
				if (strstr(tempIpAddress, "192"))
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
				}
			}
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
#endif
}

void macUdpMulcast::macUdp_send(const QString &ip, int port, QByteArray * data)
{
	uMulSocket->writeDatagram(data->data(), QHostAddress(ip), port);
}

void macUdpMulcast::receiveMul()
{
	McuInfo_t mcu;
	bool err = false;
	u8 t;
	u8 crc = 0;

	if (uMulSocket != NULL)
	{
		while (uMulSocket->hasPendingDatagrams())
		{
			//qDebug() << "udp mulcast hasPendingDatagrams ...";

			quint32 datalen = uMulSocket->pendingDatagramSize();
			QByteArray datagram;
			datagram.resize(uMulSocket->pendingDatagramSize());
			uMulSocket->readDatagram(datagram.data(), datagram.size());
			//qDebug() << "Mulcast get len;" << datalen << ", data:" << datagram.toHex();
			
			// 进行处理的一些操作 8-be 8-aa 2-len 1-packettype len-() 1-crc 8-55 8-ed
			for (int i = 0;i < 8;i++)
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
				emit signalNewLeaf(&mcu);
				emit signalNewWidget(&mcu);
			}			

WEERROR:
			err = false;
		}
	}
}
#endif
