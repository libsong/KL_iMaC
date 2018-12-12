#include "shadow_widget.h"

macShadowWidget::macShadowWidget(QWidget *parent)
	: QDialog(parent)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	setAttribute(Qt::WA_TranslucentBackground);

	//��ʼ��Ϊδ����������
	mouse_press = false;
}

macShadowWidget::~macShadowWidget()
{
}

void macShadowWidget::mousePressEvent(QMouseEvent *event)
{
	//ֻ�����������ƶ��͸ı��С
	if (event->button() == Qt::LeftButton)
	{
		mouse_press = true;
	}

	//�����ƶ�����
	move_point = event->globalPos() - pos();
}

void macShadowWidget::mouseReleaseEvent(QMouseEvent *event)
{
	mouse_press = false;
}

void macShadowWidget::mouseMoveEvent(QMouseEvent *event)
{
	//�ƶ�����
	if (mouse_press)
	{
		QPoint move_pos = event->globalPos();
		move(move_pos - move_point);
	}
}

void macShadowWidget::paintEvent(QPaintEvent *event)
{
	QPainterPath path;
	path.setFillRule(Qt::WindingFill);
	path.addRect(10, 10, this->width() - 20, this->height() - 20);//�� �� �� ��

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.fillPath(path, QBrush(Qt::white));

	QColor color(0, 0, 0, 50);
	for (int i = 0; i < 10; i++)
	{
		QPainterPath path;
		path.setFillRule(Qt::WindingFill);
		path.addRect(10 - i, 10 - i, this->width() - (10 - i) * 2, this->height() - (10 - i) * 2);
		color.setAlpha(150 - qSqrt(i) * 50);
		painter.setPen(color);
		painter.drawPath(path);
	}
}