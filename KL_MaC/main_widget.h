#pragma once

#include <QtWidgets/QWidget>
#include <QIcon>
#include <QStackedWidget>
#include <QTextBrowser>
#include <QDateTime>

#include "shadow_widget.h"
#include "title_widget.h"
#include "menu_button.h"
#include "tree_sys.h"
#include "LocalWidget.h"
#include "PowerSupplyBox.h"
#include "HighValtage.h"
#include "FiuNv.h"
#include "SignalAdjust.h"
#include "udp_mulcast.h"
#include "we_types.h"
#include "PowerSimuBox.h"
#include "SignalSwitchBox.h"
#include "wePlot.h"


class macMainWidget : public macShadowWidget
{
	Q_OBJECT

public:
	explicit macMainWidget(QWidget *parent = Q_NULLPTR);
	~macMainWidget();

public slots:
	void switchPageLocal(void);
	void switchPage(McuInfo_t *val);
	void makeDevWidget(void *info);
	void disTextBrower(QString dis);

signals:
	void testsignal(void*);
	void sendIpPort(const QString &ip,unsigned short port);

private:

	macTitleWidget 		*title_widget; //������
	macTreeSys			*systree;
	QStackedWidget		*showStack;

	macLocalInfo		*localInfo;
	//�豸widget��
	macPowerSupplyBox	*ppb = NULL;			//��Դ������ PowerSupplyBox 0x0100	
	int					ppbWidgetIndex = 0;
	macPs				*psb = NULL;			//��Դģ���� PowerSupplyBox 0x0101	
	int					psbWidgetIndex = 0;
	macHighValtage   	*hvs = NULL;			//��ѹģ���� HighValtageBox 0x0300	
	int					hvsWidgetIndex = 0;
	macFiuNv		   	*fiu = NULL;			//����ע���� FaultInsertUnit 0x0200	
	int					fiuWidgetIndex = 0;
	macSP		   		*sp = NULL;				//�źŵ����� SignalProcessBox 0x0400	
	int					spWidgetIndex = 0;
	macSw		   		*sw = NULL;				//�ź��л��� SignalProcessBox 0x0401	
	int					swWidgetIndex = 0;

	macUdpMulcast		*MulcastListen;

    //20180607 v1.1.0
	QTextBrowser		*m_dis;

	curvewidget         *m_heartBeat;


	//ContentWidget 		*content_widget; //����������
	//SystemTray 			*system_tray; //������
	//AboutUsDialog 		*about_us_dialog; //�������ǽ���
	//QString 			skin_name;//�����ڱ���ͼƬ������
	bool 				is_running;
	
};

class disTextBrowerTh : public QThread
{
	Q_OBJECT

signals:
	void toDisTextSlot(QString);

private:
	void run();
};

//////////////////////////////////////////////////////////////////////////
// define Object class

class SomeObject : public QObject
{
	Q_OBJECT
public:
	SomeObject(QObject* parent = 0) : QObject(parent) {}
	void callEmitSignal()  // ���ڷ����źŵĺ���   
	{
		emit someSignal();
	}
signals:
	void someSignal();
};

class SubThread : public QThread
{
	Q_OBJECT

signals:
	void giveDis(QString str);

public:
	SubThread(QObject* parent = 0) : QThread(parent) {}
	virtual ~SubThread()
	{
		if (obj != NULL) delete obj;
	}
public slots :
		// slot function connected to obj's someSignal   
		void someSlot();
public:
	SomeObject * obj;
};

class SubThread3 : public SubThread
{
	Q_OBJECT
public:
	SubThread3(QObject* parent = 0);
	// reimplement run   
	void run();
};















































