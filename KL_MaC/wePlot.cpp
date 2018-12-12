#include "wePlot.h"
#include <QHBoxLayout>
#include <time.h>

curvewidget::curvewidget(QWidget *parent) : QWidget(parent)
{
	customPlot = new QCustomPlot(this);
	customPlot->addGraph();
	customPlot->xAxis->setLabel("x");
	customPlot->yAxis->setLabel("y");
	// set axes ranges, so we see all data:
	customPlot->xAxis->setRange(-1, 1);
	customPlot->yAxis->setRange(0, 1);

	QTimer *dataTimer = new QTimer();
	connect(dataTimer, SIGNAL(timeout()), this, SLOT(SimpleDemo()));
	dataTimer->start(1000);
}
void curvewidget::SimpleDemo()
{
	static int cnt = 0;
	QVector<double> x(1), y(1);
	
	x[0] = cnt/50.0 - 1;
	y[0] = x[0] * x[0];

	customPlot->graph(0)->setData(x, y);
	customPlot->replot();

	cnt++;
	if (cnt >= 101)
	{
		cnt = 0;
	}
}
