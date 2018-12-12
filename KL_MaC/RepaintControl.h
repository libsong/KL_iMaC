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

	// 返回开关状态 - 打开：true 关闭：false
	bool isToggled() const;

	// 设置开关状态
	void setToggle(bool checked);

	// 设置背景颜色
	void setBackgroundColor(QColor color);

	// 设置选中颜色
	void setCheckedColor(QColor color);

	// 设置不可用颜色
	void setDisbaledColor(QColor color);

protected:
	// 绘制开关
	void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

	// 鼠标按下事件
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

	// 鼠标释放事件 - 切换开关状态、发射toggled()信号
	void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

	// 大小改变事件
	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

	// 缺省大小
	QSize sizeHint() const Q_DECL_OVERRIDE;
	QSize minimumSizeHint() const Q_DECL_OVERRIDE;

signals:
	// 状态改变时，发射信号
	void toggled(bool checked);

	private slots:
	// 状态切换时，用于产生滑动效果
	void onTimeout();

private:
	bool m_bChecked;         // 是否选中
	QColor m_background;     // 背景颜色
	QColor m_checkedColor;   // 选中颜色
	QColor m_disabledColor;  // 不可用颜色
	QColor m_thumbColor;     // 拇指颜色
	qreal m_radius;          // 圆角
	qreal m_nX;              // x点坐标
	qreal m_nY;              // y点坐标
	qint16 m_nHeight;        // 高度
	qint16 m_nMargin;        // 外边距
	QTimer m_timer;          // 定时器
};
#endif

/**
* 作者:feiyangqingyun(QQ:517216493) 2016-11-6
* 1:可设置开关按钮的样式 圆角矩形/内圆形/外圆形/图片
* 2:可设置选中和未选中时的背景颜色
* 3:可设置选中和未选中时的滑块颜色
* 4:可设置显示的文本
* 5:可设置滑块离背景的间隔
* 6:可设置圆角角度
*/

class QTimer;

class SwitchButton : public QWidget
{
	Q_OBJECT
public:
	enum ButtonStyle {
		ButtonStyle_Rect = 0,     //圆角矩形
		ButtonStyle_CircleIn = 1, //内圆形
		ButtonStyle_CircleOut = 2,//外圆形
		ButtonStyle_Image = 3     //图片
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
	bool checked;               //是否选中
	ButtonStyle buttonStyle;    //开关按钮样式

	QColor bgColorOff;          //关闭时背景颜色
	QColor bgColorOn;           //打开时背景颜色

	QColor sliderColorOff;      //关闭时滑块颜色
	QColor sliderColorOn;       //打开时滑块颜色

	QColor textColorOff;        //关闭时文本颜色
	QColor textColorOn;         //打开时文本颜色

	QString textOff;            //关闭时显示的文字
	QString textOn;             //打开时显示的文字

	QString imageOff;           //关闭时显示的图片
	QString imageOn;            //打开时显示的图片

	int space;                  //滑块离背景间隔
	int rectRadius;             //圆角角度

	int step;                   //每次移动的步长
	int startX;                 //滑块开始X轴坐标
	int endX;                   //滑块结束X轴坐标
	QTimer *timer;              //定时器绘制

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
	//设置是否选中
	void setChecked(bool checked);
	//设置风格样式
	void setButtonStyle(ButtonStyle buttonStyle);

	//设置背景颜色
	void setBgColor(QColor bgColorOff, QColor bgColorOn);
	//设置滑块颜色
	void setSliderColor(QColor sliderColorOff, QColor sliderColorOn);
	//设置文本颜色
	void setTextColor(QColor textColorOff, QColor textColorOn);

	//设置文本
	void setText(QString textOff, QString textOn);

	//设置背景图片
	void setImage(QString imageOff, QString imageOn);

	//设置间隔
	void setSpace(int space);
	//设置圆角角度
	void setRectRadius(int rectRadius);

signals:
	void checkedChanged(bool checked,int idx);
};


//动画按钮
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