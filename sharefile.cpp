#include "client.h"
#include "index.h"
#include "sharefile.h"
#include "ui_sharefile.h"

ShareFile::ShareFile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShareFile)
{
    ui->setupUi(this);
}

void ShareFile::updateFriend_LW()
{
    ui->listWidget->clear();
    QListWidget* friend_LW=Index::getInstance().getFriend()->getFriend_LW();
    for(int i=0;i<friend_LW->count();i++){
        QListWidgetItem* newItem=new QListWidgetItem(*friend_LW->item(i));
        ui->listWidget->addItem(newItem);
    }
}

ShareFile::~ShareFile()
{
    delete ui;
}

void ShareFile::on_allSelected_PB_clicked()
{
    for(int i=0;i<ui->listWidget->count();i++){
        ui->listWidget->item(i)->setSelected(true);
    }
}

void ShareFile::on_cancelSelected_PB_clicked()
{
    for(int i=0;i<ui->listWidget->count();i++){
        ui->listWidget->item(i)->setSelected(false);
    }
}

void ShareFile::on_ok_PB_clicked()
{
    QList<QListWidgetItem*> pItem=ui->listWidget->selectedItems();
    int friendnum=pItem.size();
    PDU* pdu=mkPDU(ENUM_MSG_TYPE_SHARE_FILE_REQUSET,friendnum*32+m_strShareFilePath.toStdString().size()+1);
    memcpy(pdu->caData,Client::getInstance().loginName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,&friendnum,32);
    for(int i=0;i<friendnum;i++){
        memcpy(pdu->caMsg+i*32,pItem.at(i)->text().toStdString().c_str(),32);
    }
    memcpy(pdu->caMsg+friendnum*32,m_strShareFilePath.toStdString().c_str(),m_strShareFilePath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}
