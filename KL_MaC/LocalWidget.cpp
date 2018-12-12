#include "LocalWidget.h"
#include "we_types.h"

macLocalInfo::macLocalInfo(QGroupBox *parent)
	: QGroupBox(parent)
{
	//setWindowTitle("SHOW");//TODO û������
	/*setFixedHeight(500);
	setFixedWidth(400);*/
	this->setTitle(weChinese2LocalCode("ϵͳ��Ϣ"));

	vLayout_local = new QVBoxLayout;
	m_linfoRowCount = 0;

	m_linfo = new QTableWidget(this);
	//m_linfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_linfo->setColumnCount(2);
	m_linfo->setShowGrid(false);//ȥ�������
	QHeaderView *head = m_linfo->verticalHeader();
	head->setHidden(true);//�����к�
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
	if (m_linfoRowCount%2 == 0)//ͬһ�����лᷢ�����ź�����䵥Ԫ�񣬱����ظ�������
	{
		m_linfo->setRowCount(m_linfoRowCount/2 + 1);
	}
	m_linfoRowCount++;

	m_linfo->setItem(row, column, new QTableWidgetItem(text));	

	m_linfo->item(row, column)->setFlags(m_linfo->item(row, column)->flags() & ~Qt::ItemIsEditable);//���ɱ༭
	m_linfo->item(row, column)->setFlags(m_linfo->item(row, column)->flags() & ~Qt::ItemIsSelectable);//����ѡ��
}
