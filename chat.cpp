#include "chat.h"
#include "client.h"
#include "protocol.h"
#include "ui_chat.h"

void Chat::upDateShowTE(QString strMSG)
{
    ui->show_TE->append(strMSG);
}

void Chat::clearShowTE()
{
    ui->show_TE->clear();
}

Chat::Chat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);
}

Chat::~Chat()
{
    delete ui;
}

void Chat::on_send_PB_clicked()
{
    QString strMSG=ui->input_LE->text();
    this->upDateShowTE(QString("%1:%2").arg(Client::getInstance().loginName.toStdString().c_str()).arg(strMSG.toStdString().c_str()));
    if(strMSG.isEmpty()){
        return;
    }
    PDU* pdu=mkPDU(ENUM_MSG_TYPE_CHAT_REQUSET,strMSG.toStdString().size()+1);
    memcpy(pdu->caData,Client::getInstance().loginName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,strChatName.toStdString().c_str(),32);
    qDebug()<<"strMSG.toStdString().size"<<strMSG.toStdString().size()
            <<"strMSG.toStdString().length"<<strMSG.toStdString().length();
    memcpy(pdu->caMsg,strMSG.toStdString().c_str(),strMSG.toStdString().size());
    Client::getInstance().sendMsg(pdu);
    ui->input_LE->clear();
}
