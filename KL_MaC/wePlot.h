#pragma once

#include <QWidget>
#include <QTimer>
#include"qcustomplot.h"

class curvewidget : public QWidget
{
	Q_OBJECT
public:
	explicit curvewidget(QWidget *parent = 0);
	QCustomPlot *customPlot;

public slots :
	void SimpleDemo();

private:
	QTimer dataTimer;

};
