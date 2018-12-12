#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>

#include "menu_button.h"

class macTitleWidget : public QWidget
{
	Q_OBJECT

public:
	explicit macTitleWidget(QWidget *parent = 0);
	~macTitleWidget();

protected:

	// 双击标题栏进行界面的最大化/还原
	virtual void mouseDoubleClickEvent(QMouseEvent *event);

	// 进行鼠界面的拖动
	virtual void mousePressEvent(QMouseEvent *event);

	// 设置界面标题与图标
	virtual bool eventFilter(QObject *obj, QEvent *event);

	private slots:

	// 进行最小化、最大化/还原、关闭操作
	void onClicked();

signals:
	void showMainMenu();

private:

	// 最大化/还原
	void updateMaximize();

private:
	QLabel *m_pIconLabel;
	QLabel *m_pTitleLabel;
	QPushButton *m_pMenuButton;
	QPushButton *m_pMinimizeButton;
	QPushButton *m_pMaximizeButton;
	QPushButton *m_pCloseButton;

	macMenuButton *m_pMenu;
};
