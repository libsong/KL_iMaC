#pragma once

#include <QMenu>
#include <QMenuBar>
#include <QApplication>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPoint>
#include <QMouseEvent>
#include <QDebug>
#include <QDialog>
#include <QSettings>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QAction>
#include <QProcess>
#include <QDesktopServices>
#include <QVBoxLayout>

#include "we_shell.h"
#include "we_types.h"

class macMenuButton : public QPushButton
{
	Q_OBJECT

public:
	explicit macMenuButton(QWidget *parent = 0);
	~macMenuButton();

protected slots:
	void showMenu();
	void actionSlots();
	void aboutDialog();
	void useHelp();
	void update();
	//
	void makeSsh(QString ip, QString usr, QString pwd);


signals:
	void showSettingDialog();
	void showNewCharacter();
	void showAboutUs();

private:
	void createActions();

private:

	QMenu *menu;

	QMenu *fileMenu;
	QAction *actionNew;
	QAction *actionOpen;
	QAction *actionSave;
	QAction *actionQuit;

	QMenu	*helpMenu;
	QAction *actionAbout;
	QAction *actionHelp;
	QAction *m_action_check_update; //检查更新

	QMenu *editMenu;
	QAction *actionSsh;
	QAction *actionRedo;
	QAction *actionCut;
	QAction *actionCopy;
	QAction *actionRst;
	QAction *actionSelectAll;

	//QAction *action_setting; //设置
	//QAction *action_new_character; //新版特性
	
	//QAction *action_change_company; //切换为企业版
	//QAction *action_help_online; //在线帮助
	//QAction *action_platform_help; //论坛求助
	//QAction *action_login_home; //360网站
	//QAction *action_protect; //隐私保护
	//QAction *action_about_us; //关于我们

};

class macLogin : public QDialog
{
	Q_OBJECT

public:
	macLogin (QWidget *parent = Q_NULLPTR);
	~macLogin();

	void  loadcfg();
	void  savecfg();

	void SAVE_CFG_VARIANT(const QString &key, const QVariant &value);
	QString LOAD_CFG_VARIANT(const QString &key);

signals:
	void makeSshTerminate(QString ip, QString usr, QString pwd);

public slots:
	void on_Rem_Passwd_clicked();
	void on_Autoin_Log_clicked();
	void on_btn_login_clicked();
	void QuitLogin();

private:
	QLabel		*m_nameUsr;
	QLabel		*m_namePwd;
	QLabel		*m_nameIp;

	QLineEdit   *m_lineeditPwd;
	QLineEdit   *m_lineeditUser;
	QLineEdit   *m_lineeditIp;

	QCheckBox	*m_checkPwd;
	QCheckBox	*m_checklogin;

	QPushButton *m_btnLoginNo;
	QPushButton *m_btnLoginYes;

	QString     Content_usr;
	QString     Content_pwd;
	QString     Content_ip;
	QString     Content_remeberPasswd;
	QString     Content_autologin;

	macLogin	*m_sshlogin;
};

