#include "menu_button.h"
#include <QDir>
#include <QMessageBox>

extern QList<QString> g_disText;

macMenuButton::macMenuButton(QWidget *parent /*= 0*/)
	: QPushButton(parent)
{
	//https://blog.csdn.net/heaven_evil/article/details/78601478
	//https://blog.csdn.net/u011417605/article/details/50929986
	setMinimumHeight(20);
	setMaximumWidth(50);
	setContextMenuPolicy(Qt::CustomContextMenu);
	setFocusPolicy(Qt::NoFocus);

	//�˵��� https://blog.csdn.net/pigautumn/article/details/9000350
	//menubar = new QMenuBar(this);

	//�ļ�
	menu = new QMenu;

	//fileMenu = menu->addMenu("File");
	//actionNew = fileMenu->addAction("New");
	//actionOpen = fileMenu->addAction("Open");
	//actionSave = fileMenu->addAction("Save");

	//�༭
	//editMenu = menu->addMenu(weChinese2LocalCode("����"));
	//actionSsh = editMenu->addAction(weChinese2LocalCode("������ssh"),this,SLOT(actionSlots()));
	//actionRedo = editMenu->addAction(weChinese2LocalCode("�ļ�����sftp"));
	//editMenu->addSeparator();
	//actionCut = editMenu->addAction("Cut");
	//actionCopy = editMenu->addAction("Copy");
	//actionSelectAll = editMenu->addAction("Select All");

	//���ڡ�����
	menu->addSeparator();
	helpMenu = menu->addMenu(weChinese2LocalCode("����")); 
	actionAbout = helpMenu->addAction(weChinese2LocalCode("���� ..."), this, SLOT(aboutDialog()));
	actionHelp = helpMenu->addAction(weChinese2LocalCode("KL iMaC ʹ��˵��"),this,SLOT(useHelp()));
	m_action_check_update = helpMenu->addAction(weChinese2LocalCode("�������"),this,SLOT(update()));

	//�˳�
	menu->addSeparator();
	actionRst = menu->addAction(weChinese2LocalCode("����"), this, SLOT(actionSlots()));//https://blog.csdn.net/gatieme/article/details/50374563
	actionQuit = menu->addAction(weChinese2LocalCode("�˳�"), qApp, SLOT(quit()));

	connect(this, SIGNAL(clicked()), this, SLOT(showMenu()));
}

macMenuButton::~macMenuButton()
{
	delete menu;
}

void macMenuButton::showMenu()
{
	QPoint pt = QCursor::pos();
	menu->exec(pt);
}

void macMenuButton::actionSlots()
{
	QAction *act = (QAction *)sender();
	QString actName = act->text();

	if (actName == weChinese2LocalCode("������ssh"))
	{
		macLogin *sshlogin = new macLogin(this);
		connect(sshlogin, SIGNAL(makeSshTerminate(QString, QString, QString)),\
			this, SLOT(makeSsh(QString, QString, QString)));
		
		sshlogin->show();
	}	

	if (actName == weChinese2LocalCode("����"))
	{
		qApp->closeAllWindows();

		QProcess::startDetached(qApp->applicationFilePath(),QStringList());
	}
}

void macMenuButton::aboutDialog()
{
	QDialog *about = new QDialog(this);
	about->setWindowFlags(about->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	about->setAttribute(Qt::WA_DeleteOnClose, true);
	about->setFixedWidth(220);
	about->setFixedHeight(80);

	QFont font("arial", 10, 75);
	QLabel *company = new QLabel;
	company->setFont(font);
	company->setText(weChinese2LocalCode("�Ϻ�������Ϣ���̹ɷ����޹�˾"));
	QLabel *copyright = new QLabel;	
	copyright->setFont(font);
	copyright->setText(QString("< font  color=#e9a96f;>Copyright &#169; 2018</font>"));
	QLabel *version = new QLabel;

	HRSRC hsrc = FindResource(0, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
	HGLOBAL hgbl = LoadResource(0, hsrc);
	BYTE *pBt = (BYTE *)LockResource(hgbl);
	VS_FIXEDFILEINFO* pFinfo = (VS_FIXEDFILEINFO*)(pBt + 40);
	QString valStr;
	valStr.sprintf(("Pv%d.%d - Fv%d.%d.%d.%d"), 
		(pFinfo->dwProductVersionMS >> 16) & 0xff, 
		(pFinfo->dwProductVersionMS) & 0xff,
		(pFinfo->dwFileVersionMS >> 16) & 0xFF,
		(pFinfo->dwFileVersionMS) & 0xFF,
		(pFinfo->dwFileVersionLS >> 16) & 0xFF,
		(pFinfo->dwFileVersionLS) & 0xFF);

	version->setText(valStr);

	QVBoxLayout *lyt = new QVBoxLayout;
	lyt->addWidget(company);
	lyt->addWidget(copyright);
	lyt->addWidget(version);

	about->setLayout(lyt);
	about->show();
}

void macMenuButton::useHelp()	//open folder to pdf & vedio
{
	QString  path = "file:" + QCoreApplication::applicationDirPath() + "/doc";
	QDesktopServices::openUrl(QUrl(path,QUrl::TolerantMode));

	g_disText << path;
}

void macMenuButton::update() //�������ظ���
{
	QMessageBox::StandardButton rb = QMessageBox::question(NULL, weChinese2LocalCode("������ʾ"), weChinese2LocalCode("�����Ҫ�ر�"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	if (rb == QMessageBox::Yes)
	{
		QString  path = QCoreApplication::applicationDirPath() + "/update/AutoUpdater.exe";
		QProcess::startDetached(path);

		qApp->quit();
	}	
}

void macMenuButton::makeSsh(QString ip, QString usr, QString pwd)
{
	macShell * ssh = new macShell();
	ssh->saveSshObjInf(ip,usr,pwd);
	ssh->show();
}

macLogin::macLogin(QWidget *parent)
	: QDialog(parent)
{
	m_nameUsr = new QLabel(weChinese2LocalCode("�û���"));
	m_namePwd = new QLabel(weChinese2LocalCode("�� ��"));
	m_nameIp = new QLabel(weChinese2LocalCode("IP��ַ"));;

	m_lineeditPwd = new QLineEdit;
	m_lineeditPwd->setText("redhat");
	m_lineeditUser = new QLineEdit;
	m_lineeditUser->setText("root");
	m_lineeditIp = new QLineEdit;
	m_lineeditIp->setText("192.168.1.100");

	m_checkPwd = new QCheckBox(weChinese2LocalCode("��ס��������"));
	connect(m_checkPwd, SIGNAL(clicked()), this,SLOT(on_Rem_Passwd_clicked()));
	m_checklogin = new QCheckBox(weChinese2LocalCode("�Զ���¼"));
	connect(m_checklogin, SIGNAL(clicked()), this, SLOT(on_Autoin_Log_clicked()));

	m_btnLoginNo = new QPushButton(weChinese2LocalCode("ȡ��"));
	connect(m_btnLoginNo, SIGNAL(clicked()), this, SLOT(QuitLogin()));
	m_btnLoginYes = new QPushButton(weChinese2LocalCode("�� ¼"));
	connect(m_btnLoginYes, SIGNAL(clicked()), this, SLOT(on_btn_login_clicked()));

	QVBoxLayout *vLayout = new QVBoxLayout;
	vLayout->addWidget(m_nameIp);
	vLayout->addWidget(m_lineeditIp);
	vLayout->addWidget(m_nameUsr);
	vLayout->addWidget(m_lineeditUser);
	vLayout->addWidget(m_namePwd);
	vLayout->addWidget(m_lineeditPwd);	
	vLayout->addWidget(m_checkPwd);
	//vLayout->addWidget(m_checklogin);
	vLayout->addWidget(m_btnLoginYes);
	vLayout->addWidget(m_btnLoginNo);	

	setLayout(vLayout);

	loadcfg();
}

macLogin::~macLogin()
{

}

/**��ѡ��ס����,����һ����־λ*/
void macLogin::on_Rem_Passwd_clicked()
{
	if (m_checkPwd->isChecked())
	{
		Content_remeberPasswd = "true";
	}
	else
	{
		m_lineeditPwd->clear();
		Content_remeberPasswd = "false";
	}
}


/**��ѡ�Զ���¼������һ����־λ*/
void macLogin::on_Autoin_Log_clicked()
{
	if (m_checklogin->isChecked())
	{
		Content_autologin = "true";
	}
	else
	{
		Content_autologin = "false";
	}
}

/**�����¼��Ϣ**/
void macLogin::savecfg()
{
	//QSettings cfg("sshlogin_config.ini", QSettings::IniFormat);
	Content_usr = m_lineeditUser->text();
	Content_pwd = m_lineeditPwd->text();
	Content_ip = m_lineeditIp->text();

	SAVE_CFG_VARIANT("Content_usr", Content_usr);
	SAVE_CFG_VARIANT("Content_pwd",Content_pwd);
	SAVE_CFG_VARIANT("Content_ip", Content_ip);
	SAVE_CFG_VARIANT("Content_autologin", Content_autologin);
	SAVE_CFG_VARIANT("Content_remeberPasswd", Content_remeberPasswd);

	//cfg.sync();
}
/**��ʼ��ʱ��ʾ��¼��Ϣ**/
void macLogin::loadcfg()
{
	//QSettings cfg("sshlogin_config.ini", QSettings::IniFormat);

	Content_usr = LOAD_CFG_VARIANT("Content_usr");
	Content_pwd = LOAD_CFG_VARIANT("Content_pwd");
	Content_ip = LOAD_CFG_VARIANT("Content_ip");
	Content_autologin = LOAD_CFG_VARIANT("Content_autologin");
	Content_remeberPasswd = LOAD_CFG_VARIANT("Content_remeberPasswd");

	m_lineeditUser->setText(Content_usr);
	m_lineeditPwd->setText(Content_pwd);
	m_lineeditIp->setText(Content_ip);

	if (Content_remeberPasswd == "true")
	{
		m_checkPwd->setChecked(true);
	}
	if (Content_autologin == "true")
	{
		m_checklogin->setChecked(true);
	}
}
void macLogin::SAVE_CFG_VARIANT(const QString &key, const QVariant &value)
{
	QSettings settings;
	settings.setIniCodec("UTF-8");
	//Qt��ʹ��QSettings���дini�ļ�
	//QSettings���캯���ĵ�һ��������ini�ļ���·��,�ڶ���������ʾ���ini�ļ�,��������������ȱʡ
	QSettings *configIniWrite;
	configIniWrite = new QSettings("sshlogin_config.ini", QSettings::IniFormat);
	//��ini�ļ���д������,setValue���������������Ǽ�ֵ��
	//��ini�ļ��ĵ�һ����д������,ip���µĵ�һ������
	configIniWrite->setIniCodec("UTF-8");//"GBK"
	configIniWrite->setValue(key, value);
	//д����ɺ�ɾ��ָ��
	delete configIniWrite;
	settings.sync();
}
QString macLogin::LOAD_CFG_VARIANT(const QString & key)
{
	QSettings *configIniRead = new QSettings("sshlogin_config.ini", QSettings::IniFormat);
	if (NULL == configIniRead)
	{
		//QMessageBox::information(this,"��ʾ","��cfg.ini�ļ�ʧ�ܣ�");
		qDebug() << "open sshlogin_config.ini file fail,please check the cfg.ini file is there ok?";
		return NULL;
	}


	configIniRead->setIniCodec("UTF-8");//"GBK"
										//����ȡ����ini�ļ�������QString�У���ȡֵ��Ȼ��ͨ��toString()����ת����QString����	
	QString ipResult = configIniRead->value(key).toString();//("/zk_cfg_info/zk_addr")
															//��ӡ�õ��Ľ��
	qDebug() << ipResult;
	//��������ɺ�ɾ��ָ��
	delete configIniRead;
	return ipResult;
}
/**��¼��ť**/
void macLogin::on_btn_login_clicked()
{
	savecfg();
	emit makeSshTerminate(Content_ip, Content_usr, Content_pwd);
	close();
}

void macLogin::QuitLogin()
{
	close();
}
