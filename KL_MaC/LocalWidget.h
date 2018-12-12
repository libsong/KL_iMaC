#pragma once

#include <QGroupBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QHeaderView>

//https://blog.csdn.net/polokang/article/details/6696982

class macLocalInfo : public QGroupBox
{
	Q_OBJECT

public:
	macLocalInfo(QGroupBox *parent = 0);
	~macLocalInfo();



public slots:
	void addNewItem(int infotype,int row, int column, const QString &text);

private:
	int				m_linfoRowCount;

	QTableWidget	*m_linfo;
	QVBoxLayout		*vLayout_local;
};
