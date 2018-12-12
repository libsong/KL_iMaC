#include <QApplication>
#include <QTextCodec>
#include <QSharedMemory>
#include <QSplashScreen>
#include <QDateTime>

#include "main_widget.h"
#include "wePlot.h"

//void myMessageOutput(QtMsgType type, const QMessageLogContext & context, const QString & msg)
//{
//	// 加锁
//	static QMutex mutex;
//	mutex.lock();
//
//	QByteArray localMsg = msg.toLocal8Bit();
//
//	QString strMsg("");
//	switch (type)
//	{
//	case QtDebugMsg:
//		strMsg = QString("Debug:");
//		break;
//	case QtWarningMsg:
//		strMsg = QString("Warning:");
//		break;
//	case QtCriticalMsg:
//		strMsg = QString("Critical:");
//		break;
//	case QtFatalMsg:
//		strMsg = QString("Fatal:");
//		break;
//	}
//
//	// 设置输出信息格式
//	QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
//	QString strMessage = QString("Message:%1 File:%2  Line:%3  Function:%4  DateTime:%5")
//		.arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function).arg(strDateTime);
//
//	// 输出信息至文件中（读写、追加形式）
//	QFile file("log.txt");
//	file.open(QIODevice::ReadWrite | QIODevice::Append);
//	QTextStream stream(&file);
//	stream << strMessage << "\r\n";
//	file.flush();
//	file.close();
//
//	// 解锁
//	mutex.unlock();
//}


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	//安装消息处理
	//qInstallMessageHandler(myMessageOutput);

	//
	QTextCodec *codec = QTextCodec::codecForName("System");
	QTextCodec::setCodecForLocale(codec);
	
	//使用QSharedMemory防止多开  
	QSharedMemory shared_memory;
	shared_memory.setKey(QString("main_window"));
	if (shared_memory.attach())
	{
		return 0;
	}

	if (shared_memory.create(1))
	{
		//启动画面
		QPixmap pixmap(":/KL_MaC/Resources/img/load/loading.png");
		QSplashScreen screen(pixmap);
		screen.showMessage(weChinese2LocalCode("程序启动中，请稍候..."), Qt::AlignTop | Qt::AlignRight, Qt::red);
		screen.show();		
		QDateTime n = QDateTime::currentDateTime();
		QDateTime now;
		do {
			now = QDateTime::currentDateTime();
			a.processEvents();
		} while (n.secsTo(now) <= 1);//5为需要延时的秒数

		//主窗口
		macMainWidget w;

		//信息显示窗口
		disTextBrowerTh * dis = new disTextBrowerTh;
		QObject::connect(dis, SIGNAL(toDisTextSlot(QString)),&w, SLOT(disTextBrower(QString)));
		dis->start();

		w.show();
		screen.finish(&w);//启动画面在窗口w创建完成以后结束  

		return a.exec();
	}
}
