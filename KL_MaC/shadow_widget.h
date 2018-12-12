#pragma once

#include <QDialog>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <qmath.h>

class macShadowWidget : public QDialog
{
	Q_OBJECT

public:
	explicit macShadowWidget(QWidget *parent = 0);
	~macShadowWidget();

protected:

	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	virtual void paintEvent(QPaintEvent *event);

private:

	QPoint move_point; //�ƶ��ľ���
	bool mouse_press; //����������

};


