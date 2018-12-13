#include "we_types.h"
#include <QByteArray>
#include <QTime>

//https://blog.csdn.net/fantasker/article/details/73199267
void stringToHtmlFilter(QString &str)
{
	//注意这几行代码的顺序不能乱，否则会造成多次替换
	str.replace("&", "&amp;");
	str.replace(">", "&gt;");
	str.replace("<", "&lt;");
	str.replace("\"", "&quot;");
	str.replace("\'", "&#39;");
	str.replace(" ", "&nbsp;");
	str.replace("\n", "<br>");
	str.replace("\r", "<br>");
}

void stringToHtml(QString &str, QColor crl)
{
	QByteArray array;
	array.append(crl.red());
	array.append(crl.green());
	array.append(crl.blue());
	QString strC(array.toHex());
	str = QString("<span style=\" color:#%1;\">%2</span>").arg(strC).arg(str);
}

void Delay_MSec_Suspend(unsigned int msec)
{

	QTime _Timer = QTime::currentTime();

	QTime _NowTimer;
	do {
		_NowTimer = QTime::currentTime();
	} while (_Timer.msecsTo(_NowTimer) <= msec);

}


