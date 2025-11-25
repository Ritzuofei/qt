#include "client.h"
#include "onlineuser.h"
#include "protocol.h"
#include "ui_onlineuser.h"

OnlineUser::OnlineUser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnlineUser)
{
    ui->setupUi(this);
}

OnlineUser::~OnlineUser()
{
    delete ui;
}

void OnlineUser::update_LW(QStringList userList)
{
    ui->listWidget->clear();
    ui->listWidget->addItems(userList);
}

void OnlineUser::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    PDU* pdu=mkPDU(ENUM_MSG_TYPE_ADD_FRIEND_REQUSET);
    memcpy(pdu->caData,Client::getInstance().loginName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,item->text().toStdString().c_str(),32);
    Client::getInstance().sendMsg(pdu);
}
