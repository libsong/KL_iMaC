#include "title_widget.h"
#include <QDebug>
#include "push_button.h"

#ifdef Q_OS_WIN
#pragma comment(lib, "user32.lib")
#include <qt_windows.h>
#endif

macTitleWidget::macTitleWidget(QWidget *parent)
	: QWidget(parent)
{

	//关于标题 https://blog.csdn.net/liang19890820/article/details/50555298

	//setFixedHeight(50);
	//setStyleSheet("background-color:lightgray;");
	setAttribute(Qt::WA_TranslucentBackground, true);//设置透明

	QIcon icon;
	QFont font("Microsoft YaHei", 10, 63); //第一个属性是字体（微软雅黑），第二个是大小，第三个是加粗（权重是75） 
	//常见权重
	//QFont::Light - 25 高亮
	//QFont::Normal - 50 正常
	//QFont::DemiBold - 63 半粗体
	//QFont::Bold - 75 粗体
	//QFont::Black - 87 黑体

	m_pIconLabel = new QLabel(this);
	m_pIconLabel->setFixedSize(20, 20);
	m_pIconLabel->setScaledContents(true);

	m_pTitleLabel = new QLabel(this);
	m_pTitleLabel->setFont(font);
	m_pTitleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	m_pMenu = new macMenuButton(this);
	icon.addFile(tr(":/KL_MaC/set"));
	m_pMenu->setIcon(icon);

	m_pMinimizeButton = new PushButton(this);
	icon.addFile(tr(":/KL_MaC/titlemin"));
	m_pMinimizeButton->setIcon(icon);

	m_pMaximizeButton = new PushButton(this);
	icon.addFile(tr(":/KL_MaC/titlemax"));
	m_pMaximizeButton->setIcon(icon);

	m_pCloseButton = new PushButton(this);
	icon.addFile(tr(":/KL_MaC/titleclose"));
	m_pCloseButton->setIcon(icon);

	m_pMenu->setFixedSize(27,20);
	m_pMinimizeButton->setFixedSize(27, 20);
	m_pMaximizeButton->setFixedSize(27, 20);
	m_pCloseButton->setFixedSize(27, 20);

	m_pTitleLabel->setObjectName("whiteLabel");
	m_pMenu->setObjectName("mebuButton");
	m_pMinimizeButton->setObjectName("minimizeButton");
	m_pMaximizeButton->setObjectName("maximizeButton");
	m_pCloseButton->setObjectName("closeButton");

	m_pMinimizeButton->setToolTip("Minimize");
	m_pMaximizeButton->setToolTip("Maximize");
	m_pCloseButton->setToolTip("Close");
	m_pMenu->setToolTip("MainMenu");

	QHBoxLayout *pLayout = new QHBoxLayout(this);
	pLayout->addWidget(m_pIconLabel);
	pLayout->addWidget(m_pTitleLabel);
	pLayout->addWidget(m_pMenu);
	pLayout->addWidget(m_pMinimizeButton);
	pLayout->addWidget(m_pMaximizeButton);
	pLayout->addWidget(m_pCloseButton);
	pLayout->setSpacing(0);
	pLayout->setContentsMargins(1, 0, 1, 0);

	setLayout(pLayout);

	connect(m_pMinimizeButton, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
	connect(m_pMaximizeButton, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
	connect(m_pCloseButton, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
	connect(m_pMenu, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
}

macTitleWidget::~macTitleWidget()
{

}

void macTitleWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	Q_UNUSED(event);

	emit m_pMaximizeButton->clicked();
}

void macTitleWidget::mousePressEvent(QMouseEvent *event)
{
#ifdef Q_OS_WIN
	if (ReleaseCapture())
	{
		QWidget *pWindow = this->window();
		if (pWindow->isTopLevel())
		{
			SendMessage(HWND(pWindow->winId()), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0);
		}
	}
	event->ignore();
#else
#endif
}

bool macTitleWidget::eventFilter(QObject *obj, QEvent *event)
{
#if 1
	switch (event->type())
	{
	case QEvent::WindowTitleChange:
	{
		QWidget *pWidget = qobject_cast<QWidget *>(obj);
		if (pWidget)
		{
			m_pTitleLabel->setText(pWidget->windowTitle());
			return true;
		}
	}
	case QEvent::WindowIconChange:
	{
		QWidget *pWidget = qobject_cast<QWidget *>(obj);
		if (pWidget)
		{
			QIcon icon = pWidget->windowIcon();
			m_pIconLabel->setPixmap(icon.pixmap(m_pIconLabel->size()));
			return true;
		}
	}
	case QEvent::WindowStateChange:
	case QEvent::Resize:
		updateMaximize();
		return true;
	}

	return QWidget::eventFilter(obj, event);
#endif
}

void macTitleWidget::onClicked()
{
	QPushButton *pButton = qobject_cast<QPushButton *>(sender());
	QWidget *pWindow = this->window();
	if (pWindow->isTopLevel())
	{
		if (pButton == m_pMinimizeButton)
		{
			pWindow->showMinimized();
		}
		else if (pButton == m_pMaximizeButton)
		{
			pWindow->isMaximized() ? pWindow->showNormal() : pWindow->showMaximized();
		}
		else if (pButton == m_pCloseButton)
		{
			pWindow->close();
		}
		else if (pButton == m_pMenuButton)
		{
			qDebug("m_pMenuButton click .");
			emit showMainMenu();
		}
	}
}

void macTitleWidget::updateMaximize()
{
	QWidget *pWindow = this->window();
	if (pWindow->isTopLevel())
	{
		bool bMaximize = pWindow->isMaximized();
		if (bMaximize)
		{
			m_pMaximizeButton->setToolTip(tr("Restore"));
			m_pMaximizeButton->setProperty("maximizeProperty", "restore");
		}
		else
		{
			m_pMaximizeButton->setProperty("maximizeProperty", "maximize");
			m_pMaximizeButton->setToolTip(tr("Maximize"));
		}

		m_pMaximizeButton->setStyle(QApplication::style());
	}
}