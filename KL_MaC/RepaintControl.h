#pragma once

#include <QWidget>
#include <QTimer>
#include <QPushButton>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QPainter>

#if 0
class SwitchControl : public QWidget
{
	Q_OBJECT

public:
	explicit SwitchControl(QWidget *parent = 0);

	// ���ؿ���״̬ - �򿪣�true �رգ�false
	bool isToggled() const;

	// ���ÿ���״̬
	void setToggle(bool checked);

	// ���ñ�����ɫ
	void setBackgroundColor(QColor color);

	// ����ѡ����ɫ
	void setCheckedColor(QColor color);

	// ���ò�������ɫ
	void setDisbaledColor(QColor color);

protected:
	// ���ƿ���
	void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

	// ��갴���¼�
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

	// ����ͷ��¼� - �л�����״̬������toggled()�ź�
	void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

	// ��С�ı��¼�
	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

	// ȱʡ��С
	QSize sizeHint() const Q_DECL_OVERRIDE;
	QSize minimumSizeHint() const Q_DECL_OVERRIDE;

signals:
	// ״̬�ı�ʱ�������ź�
	void toggled(bool checked);

	private slots:
	// ״̬�л�ʱ�����ڲ�������Ч��
	void onTimeout();

private:
	bool m_bChecked;         // �Ƿ�ѡ��
	QColor m_background;     // ������ɫ
	QColor m_checkedColor;   // ѡ����ɫ
	QColor m_disabledColor;  // ��������ɫ
	QColor m_thumbColor;     // Ĵָ��ɫ
	qreal m_radius;          // Բ��
	qreal m_nX;              // x������
	qreal m_nY;              // y������
	qint16 m_nHeight;        // �߶�
	qint16 m_nMargin;        // ��߾�
	QTimer m_timer;          // ��ʱ��
};
#endif

/**
* ����:feiyangqingyun(QQ:517216493) 2016-11-6
* 1:�����ÿ��ذ�ť����ʽ Բ�Ǿ���/��Բ��/��Բ��/ͼƬ
* 2:������ѡ�к�δѡ��ʱ�ı�����ɫ
* 3:������ѡ�к�δѡ��ʱ�Ļ�����ɫ
* 4:��������ʾ���ı�
* 5:�����û����뱳���ļ��
* 6:������Բ�ǽǶ�
*/

class QTimer;

class SwitchButton : public QWidget
{
	Q_OBJECT
public:
	enum ButtonStyle {
		ButtonStyle_Rect = 0,     //Բ�Ǿ���
		ButtonStyle_CircleIn = 1, //��Բ��
		ButtonStyle_CircleOut = 2,//��Բ��
		ButtonStyle_Image = 3     //ͼƬ
	};

	SwitchButton(QWidget *parent = 0);
	~SwitchButton();

	void mousePressEvent_WeSimu(bool check);
protected:
	void mousePressEvent(QMouseEvent *);
	void resizeEvent(QResizeEvent *);
	void paintEvent(QPaintEvent *);
	void drawBg(QPainter *painter);
	void drawSlider(QPainter *painter);
	void drawText(QPainter *painter);
	void drawImage(QPainter *painter);

private:
	bool checked;               //�Ƿ�ѡ��
	ButtonStyle buttonStyle;    //���ذ�ť��ʽ

	QColor bgColorOff;          //�ر�ʱ������ɫ
	QColor bgColorOn;           //��ʱ������ɫ

	QColor sliderColorOff;      //�ر�ʱ������ɫ
	QColor sliderColorOn;       //��ʱ������ɫ

	QColor textColorOff;        //�ر�ʱ�ı���ɫ
	QColor textColorOn;         //��ʱ�ı���ɫ

	QString textOff;            //�ر�ʱ��ʾ������
	QString textOn;             //��ʱ��ʾ������

	QString imageOff;           //�ر�ʱ��ʾ��ͼƬ
	QString imageOn;            //��ʱ��ʾ��ͼƬ

	int space;                  //�����뱳�����
	int rectRadius;             //Բ�ǽǶ�

	int step;                   //ÿ���ƶ��Ĳ���
	int startX;                 //���鿪ʼX������
	int endX;                   //�������X������
	QTimer *timer;              //��ʱ������

	int	index;

private slots:
	void updateValue();

public:
	bool getChecked()const
	{
		return checked;
	}
	ButtonStyle getButtonStyle()const
	{
		return buttonStyle;
	}

	QColor getBgColorOff()const
	{
		return bgColorOff;
	}
	QColor getBgColorOn()const
	{
		return bgColorOn;
	}

	QColor getSliderColorOff()const
	{
		return sliderColorOff;
	}
	QColor getSliderColorOn()const
	{
		return sliderColorOn;
	}

	QColor getTextColorOff()const
	{
		return textColorOff;
	}
	QColor getTextColorOn()const
	{
		return textColorOn;
	}

	QString getTextOff()const
	{
		return textOff;
	}
	QString getTextOn()const
	{
		return textOn;
	}

	QString getImageOff()const
	{
		return imageOff;
	}
	QString getImageOn()const
	{
		return imageOn;
	}

	int getSpace()const
	{
		return space;
	}
	int getRectRadius()const
	{
		return rectRadius;
	}
	void setIndex(int idx)
	{
		index = idx;
	}
	int getIndex()
	{
		return index;
	}

public slots:
	//�����Ƿ�ѡ��
	void setChecked(bool checked);
	//���÷����ʽ
	void setButtonStyle(ButtonStyle buttonStyle);

	//���ñ�����ɫ
	void setBgColor(QColor bgColorOff, QColor bgColorOn);
	//���û�����ɫ
	void setSliderColor(QColor sliderColorOff, QColor sliderColorOn);
	//�����ı���ɫ
	void setTextColor(QColor textColorOff, QColor textColorOn);

	//�����ı�
	void setText(QString textOff, QString textOn);

	//���ñ���ͼƬ
	void setImage(QString imageOff, QString imageOn);

	//���ü��
	void setSpace(int space);
	//����Բ�ǽǶ�
	void setRectRadius(int rectRadius);

signals:
	void checkedChanged(bool checked,int idx);
};


//������ť
class weAnimationPushButton : public QPushButton
{
	Q_OBJECT
	Q_PROPERTY(int alpha READ alpha WRITE setAlpha)

public:
	weAnimationPushButton(QPushButton *parent = 0);
	~weAnimationPushButton();

	QPropertyAnimation *pAnimation;

private:
	int alpha() const;
	void setAlpha(const int alpha);

private:
	int m_nAlpha;
};