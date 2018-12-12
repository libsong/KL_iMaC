#include "LocalWidget.h"
#include "we_types.h"

macLocalInfo::macLocalInfo(QGroupBox *parent)
	: QGroupBox(parent)
{
	//setWindowTitle("SHOW");//TODO 没起作用
	/*setFixedHeight(500);
	setFixedWidth(400);*/
	this->setTitle(weChinese2LocalCode("系统信息"));

	vLayout_local = new QVBoxLayout;
	m_linfoRowCount = 0;

	m_linfo = new QTableWidget(this);
	//m_linfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_linfo->setColumnCount(2);
	m_linfo->setShowGrid(false);//去除表格线
	QHeaderView *head = m_linfo->verticalHeader();
	head->setHidden(true);//隐藏行号
 	/*QStringList header;
	header << "Name" << "Value";
	m_linfo->setHorizontalHeaderLabels(header);*/
	head = m_linfo->horizontalHeader();
	head->setHidden(true);
	m_linfo->horizontalHeader()->setStretchLastSection(true);
	m_linfo->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

	vLayout_local->addWidget(m_linfo);
	setLayout(vLayout_local);
}

macLocalInfo::~macLocalInfo()
{
	if (m_linfo != NULL)
	{
		delete m_linfo;
	}

}

void macLocalInfo::addNewItem(int infotype,int row ,int column, const QString &text)
{
	if (m_linfoRowCount%2 == 0)//同一行两列会发两次信号来填充单元格，避免重复创建行
	{
		m_linfo->setRowCount(m_linfoRowCount/2 + 1);
	}
	m_linfoRowCount++;

	m_linfo->setItem(row, column, new QTableWidgetItem(text));	

	m_linfo->item(row, column)->setFlags(m_linfo->item(row, column)->flags() & ~Qt::ItemIsEditable);//不可编辑
	m_linfo->item(row, column)->setFlags(m_linfo->item(row, column)->flags() & ~Qt::ItemIsSelectable);//不可选择
}
