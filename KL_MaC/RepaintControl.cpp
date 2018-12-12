#include "RepaintControl.h"
#include <QPainter>
#include <QMouseEvent>

#if 0
// https://blog.csdn.net/liang19890820/article/details/52164289

SwitchControl::SwitchControl(QWidget *parent)
	: QWidget(parent),
	m_nHeight(16),
	m_bChecked(false),
	m_radius(8.0),
	m_nMargin(3),
	m_checkedColor(0, 150, 136),
	m_thumbColor(Qt::red),
	m_disabledColor(190, 190, 190),
	m_background(Qt::lightGray)
{
	// 鼠标滑过光标形状 - 手型
	setCursor(Qt::PointingHandCursor);

	// 连接信号槽
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

// 绘制开关
void SwitchControl::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);

	QPainter painter(this);
	painter.setPen(Qt::NoPen);
	painter.setRenderHint(QPainter::Antialiasing);

	QPainterPath path;
	QColor background;
	QColor thumbColor;
	qreal dOpacity;
	if (isEnabled()) { // 可用状态
		if (m_bChecked) { // 打开状态
			background = m_checkedColor;
			thumbColor = m_checkedColor;
			dOpacity = 0.600;
		}
		else { //关闭状态
			background = m_background;
			thumbColor = m_thumbColor;
			dOpacity = 0.800;
		}
	}
	else {  // 不可用状态
		background = m_background;
		dOpacity = 0.260;
		thumbColor = m_disabledColor;
	}
	// 绘制大椭圆
	painter.setBrush(background);
	painter.setOpacity(dOpacity);
	path.addRoundedRect(QRectF(m_nMargin, m_nMargin, width() - 2 * m_nMargin, height() - 2 * m_nMargin), m_radius, m_radius);
	painter.drawPath(path.simplified());

	// 绘制小椭圆
	painter.setBrush(thumbColor);
	painter.setOpacity(1.0);
	painter.drawEllipse(QRectF(m_nX - (m_nHeight / 2), m_nY - (m_nHeight / 2), height(), height()));
}

// 鼠标按下事件
void SwitchControl::mousePressEvent(QMouseEvent *event)
{
	if (isEnabled()) {
		if (event->buttons() & Qt::LeftButton) {
			event->accept();
		}
		else {
			event->ignore();
		}
	}
}

// 鼠标释放事件 - 切换开关状态、发射toggled()信号
void SwitchControl::mouseReleaseEvent(QMouseEvent *event)
{
	if (isEnabled()) {
		if ((event->type() == QMouseEvent::MouseButtonRelease) && (event->button() == Qt::LeftButton)) {
			event->accept();
			m_bChecked = !m_bChecked;
			emit toggled(m_bChecked);
			m_timer.start(1);
		}
		else {
			event->ignore();
		}
	}
}

// 大小改变事件
void SwitchControl::resizeEvent(QResizeEvent *event)
{
	m_nX = m_nHeight / 2;
	m_nY = m_nHeight / 2;
	QWidget::resizeEvent(event);
}

// 默认大小
QSize SwitchControl::sizeHint() const
{
	return minimumSizeHint();
}

// 最小大小
QSize SwitchControl::minimumSizeHint() const
{
	return QSize(2 * (m_nHeight + m_nMargin), m_nHeight + 2 * m_nMargin);
}

// 切换状态 - 滑动
void SwitchControl::onTimeout()
{
	if (m_bChecked) {
		m_nX += 1;
		if (m_nX >= width() - m_nHeight)
			m_timer.stop();
	}
	else {
		m_nX -= 1;
		if (m_nX <= m_nHeight / 2)
			m_timer.stop();
	}
	update();
}

// 返回开关状态 - 打开：true 关闭：false
bool SwitchControl::isToggled() const
{
	return m_bChecked;
}

// 设置开关状态
void SwitchControl::setToggle(bool checked)
{
	m_bChecked = checked;
	m_timer.start(1);
}

// 设置背景颜色
void SwitchControl::setBackgroundColor(QColor color)
{
	m_background = color;
}

// 设置选中颜色
void SwitchControl::setCheckedColor(QColor color)
{
	m_checkedColor = color;
}

// 设置不可用颜色
void SwitchControl::setDisbaledColor(QColor color)
{
	m_disabledColor = color;
}
#endif



//https://blog.csdn.net/eydwyz/article/details/72912708
SwitchButton::SwitchButton(QWidget *parent) 
	: QWidget(parent)
{
	checked = false;
	buttonStyle = ButtonStyle_Rect;

	bgColorOff = QColor(225, 225, 225);
	bgColorOn = QColor(250, 250, 250);

	sliderColorOff = QColor(100, 100, 100);
	sliderColorOn = QColor(100, 184, 255);

	textColorOff = QColor(255, 255, 255);
	textColorOn = QColor(10, 10, 10);

	textOff = "";
	textOn = "";

	imageOff = ":/image/btncheckoff1.png";
	imageOn = ":/image/btncheckon1.png";

	space = 2;
	rectRadius = 5;

	step = width() / 50;
	startX = 0;
	endX = 0;

	timer = new QTimer(this);
	timer->setInterval(5);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateValue()));

	setFont(QFont("Microsoft Yahei", this->width()/14));

	index = 0;
}

SwitchButton::~SwitchButton()
{

}

void SwitchButton::mousePressEvent_WeSimu(bool check)
{
	checked = check;
	//emit checkedChanged(checked, getIndex());

	//每次移动的步长为宽度的 50分之一
	//step = width() / 50;
	step = width() / 20;

	//状态切换改变后自动计算终点坐标
	if (checked) {
		if (buttonStyle == ButtonStyle_Rect) {
			endX = width() - width() / 2;
		}
		else if (buttonStyle == ButtonStyle_CircleIn) {
			endX = width() - height();
		}
		else if (buttonStyle == ButtonStyle_CircleOut) {
			endX = width() - height() + space;
		}
	}
	else {
		endX = 0;
	}

	timer->start();
}

void SwitchButton::mousePressEvent(QMouseEvent *)
{
	checked = !checked;
	emit checkedChanged(checked,getIndex());

	//每次移动的步长为宽度的 50分之一
	//step = width() / 50;
	step = width() /20;

	//状态切换改变后自动计算终点坐标
	if (checked) {
		if (buttonStyle == ButtonStyle_Rect) {
			endX = width() - width() / 2;
		}
		else if (buttonStyle == ButtonStyle_CircleIn) {
			endX = width() - height();
		}
		else if (buttonStyle == ButtonStyle_CircleOut) {
			endX = width() - height() + space;
		}
	}
	else {
		endX = 0;
	}

	timer->start();
}

void SwitchButton::resizeEvent(QResizeEvent *)
{
	//每次移动的步长为宽度的 50分之一
	step = width() / 50;

	//尺寸大小改变后自动设置起点坐标为终点
	if (checked) {
		if (buttonStyle == ButtonStyle_Rect) {
			startX = width() - width() / 2;
		}
		else if (buttonStyle == ButtonStyle_CircleIn) {
			startX = width() - height();
		}
		else if (buttonStyle == ButtonStyle_CircleOut) {
			startX = width() - height() + space;
		}
	}
	else {
		startX = 0;
	}

	update();
}

void SwitchButton::paintEvent(QPaintEvent *)
{
	//绘制准备工作,启用反锯齿
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	if (buttonStyle == ButtonStyle_Image) {
		//绘制图片
		drawImage(&painter);
	}
	else {
		//绘制背景
		drawBg(&painter);
		//绘制滑块
		drawSlider(&painter);
		//绘制文字
		drawText(&painter);
	}
}

void SwitchButton::drawBg(QPainter *painter)
{
	painter->save();
	painter->setPen(Qt::NoPen);

	if (!checked) {
		painter->setBrush(bgColorOff);
	}
	else {
		painter->setBrush(bgColorOn);
	}

	if (buttonStyle == ButtonStyle_Rect) {
		painter->drawRoundedRect(rect(), rectRadius, rectRadius);
	}
	else if (buttonStyle == ButtonStyle_CircleIn) {
		QRect rect(0, 0, width(), height());
		//半径为高度的一半
		int radius = rect.height() / 2;
		//圆的宽度为高度
		int circleWidth = rect.height();

		QPainterPath path;
		path.moveTo(radius, rect.left());
		path.arcTo(QRectF(rect.left(), rect.top(), circleWidth, circleWidth), 90, 180);
		path.lineTo(rect.width() - radius, rect.height());
		path.arcTo(QRectF(rect.width() - rect.height(), rect.top(), circleWidth, circleWidth), 270, 180);
		path.lineTo(radius, rect.top());

		painter->drawPath(path);
	}
	else if (buttonStyle == ButtonStyle_CircleOut) {
		QRect rect(space, space, width() - space * 2, height() - space * 2);
		painter->drawRoundedRect(rect, rectRadius, rectRadius);
	}

	painter->restore();
}

void SwitchButton::drawSlider(QPainter *painter)
{
	painter->save();
	painter->setPen(Qt::NoPen);

	if (!checked) {
		painter->setBrush(sliderColorOff);
	}
	else {
		painter->setBrush(sliderColorOn);
	}

	if (buttonStyle == ButtonStyle_Rect) {
		int sliderWidth = width() / 2 - space * 2;
		int sliderHeight = height() - space * 2;
		QRect sliderRect(startX + space, space, sliderWidth, sliderHeight);
		painter->drawRoundedRect(sliderRect, rectRadius, rectRadius);
	}
	else if (buttonStyle == ButtonStyle_CircleIn) {
		QRect rect(0, 0, width(), height());
		int sliderWidth = rect.height() - space * 2;
		QRect sliderRect(startX + space, space, sliderWidth, sliderWidth);
		painter->drawEllipse(sliderRect);
	}
	else if (buttonStyle == ButtonStyle_CircleOut) {
		QRect rect(0, 0, width() - space, height() - space);
		int sliderWidth = rect.height();
		QRect sliderRect(startX, space / 2, sliderWidth, sliderWidth);
		painter->drawEllipse(sliderRect);
	}

	painter->restore();
}

void SwitchButton::drawText(QPainter *painter)
{
	painter->save();

	if (!checked) {
		painter->setPen(textColorOff);
		painter->drawText(width() / 2, 0, width() / 2 - space, height(), Qt::AlignCenter, textOff);
	}
	else {
		painter->setPen(textColorOn);
		painter->drawText(0, 0, width() / 2 + space * 2, height(), Qt::AlignCenter, textOn);
	}

	painter->restore();
}

void SwitchButton::drawImage(QPainter *painter)
{
	painter->save();

	QPixmap pix;

	if (!checked) {
		pix = QPixmap(imageOff);
	}
	else {
		pix = QPixmap(imageOn);
	}

	//自动等比例平滑缩放居中显示
	int targetWidth = pix.width();
	int targetHeight = pix.height();
	pix = pix.scaled(targetWidth, targetHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	int pixX = rect().center().x() - targetWidth / 2;
	int pixY = rect().center().y() - targetHeight / 2;
	QPoint point(pixX, pixY);
	painter->drawPixmap(point, pix);

	painter->restore();
}

void SwitchButton::updateValue()
{
	if (checked) {
		if (startX < endX) {
			startX = startX + step;
		}
		else {
			startX = endX;
			timer->stop();
		}
	}
	else {
		if (startX > endX) {
			startX = startX - step;
		}
		else {
			startX = endX;
			timer->stop();
		}
	}

	update();
}

void SwitchButton::setChecked(bool checked)
{
	if (this->checked != checked) {
		this->checked = checked;
		emit checkedChanged(checked,getIndex());
		update();
	}
}

void SwitchButton::setButtonStyle(SwitchButton::ButtonStyle buttonStyle)
{
	this->buttonStyle = buttonStyle;
	update();
}

void SwitchButton::setBgColor(QColor bgColorOff, QColor bgColorOn)
{
	this->bgColorOff = bgColorOff;
	this->bgColorOn = bgColorOn;
	update();
}

void SwitchButton::setSliderColor(QColor sliderColorOff, QColor sliderColorOn)
{
	this->sliderColorOff = sliderColorOff;
	this->sliderColorOn = sliderColorOn;
	update();
}

void SwitchButton::setTextColor(QColor textColorOff, QColor textColorOn)
{
	this->textColorOff = textColorOff;
	this->textColorOn = textColorOn;
	update();
}

void SwitchButton::setText(QString textOff, QString textOn)
{
	this->textOff = textOff;
	this->textOn = textOn;
	update();
}

void SwitchButton::setImage(QString imageOff, QString imageOn)
{
	this->imageOff = imageOff;
	this->imageOn = imageOn;
	update();
}

void SwitchButton::setSpace(int space)
{
	this->space = space;
	update();
}

void SwitchButton::setRectRadius(int rectRadius)
{
	this->rectRadius = rectRadius;
	update();
}

//动画按钮
weAnimationPushButton::weAnimationPushButton(QPushButton *parent)
	: QPushButton(parent)
{
	pAnimation = new QPropertyAnimation();
	pAnimation->setTargetObject(this);
	pAnimation->setPropertyName("alpha");
	pAnimation->setDuration(1000);//动画持续时长 1s
	pAnimation->setKeyValueAt(0, 1);
	//pAnimation->setKeyValueAt(0.5, 100);
	pAnimation->setKeyValueAt(1, 100);
	//pAnimation->setLoopCount(-1);  //永远运行，直到stop
	pAnimation->setLoopCount(1);

	connect(this, SIGNAL(clicked(bool)), pAnimation, SLOT(start()));
}

weAnimationPushButton::~weAnimationPushButton()
{
	delete pAnimation;
}

int weAnimationPushButton::alpha() const
{
	return m_nAlpha;
}

void weAnimationPushButton::setAlpha(const int alpha)
{
	m_nAlpha = alpha;
	QString strQSS = QString("color: rgb(0, 160, 230); background-color: rgba(10, 160, 105, %1);").arg(m_nAlpha);
	//QString strQSS = QString("color: rgb(255, 0, 0); background-color: rgba(0, 255, 0, %1);").arg(m_nAlpha);
	setStyleSheet(strQSS);
}
