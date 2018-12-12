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

	// ˫�����������н�������/��ԭ
	virtual void mouseDoubleClickEvent(QMouseEvent *event);

	// �����������϶�
	virtual void mousePressEvent(QMouseEvent *event);

	// ���ý��������ͼ��
	virtual bool eventFilter(QObject *obj, QEvent *event);

	private slots:

	// ������С�������/��ԭ���رղ���
	void onClicked();

signals:
	void showMainMenu();

private:

	// ���/��ԭ
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
