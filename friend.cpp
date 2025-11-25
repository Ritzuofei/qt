#include "client.h"
#include "friend.h"
#include "protocol.h"
#include "ui_friend.h"

#include <QMessageBox>
#include <qinputdialog.h>

Friend::Friend(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Friend)
{
    ui->setupUi(this);
    m_ponlineUser=new OnlineUser;
    m_ponlineUser->hide();
    m_pChat=new Chat;
    m_pChat->hide();
}

void Friend::update_LW(QStringList list)
{
    ui->listWidget->clear();
    ui->listWidget->addItems(list);
}

void Friend::flushFriend()
{
    qDebug()<<"flushFriend";
    QString strLoginName=Client::getInstance().loginName;
    PDU* pdu=mkPDU(ENUM_MSG_TYPE_FLUSH_FRIEND_REQUSET);
    memcpy(pdu->caData,strLoginName.toStdString().c_str(),32);
    Client::getInstance().sendMsg(pdu);
}

QListWidget *Friend::getFriend_LW()
{
    QListWidget* friend_LW;
    friend_LW=ui->listWidget;
    return friend_LW;
}

Friend::~Friend()
{
    delete ui;
    delete m_ponlineUser;
    delete m_pChat;
}

void Friend::on_findUser_PB_clicked()
{
    QString strName = QInputDialog::getText(this,"查找","用户名");
    if(strName.isEmpty()){
        return;
    }
    if(strName.toStdString().size()>32){
        QMessageBox::information(this,"查找","用户名长度非法");
    }
    PDU* pdu=mkPDU(ENUM_MSG_TYPE_FIND_USER_REQUSET);
    memcpy(pdu->caData,strName.toStdString().c_str(),strName.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

void Friend::on_onlineUser_PB_clicked()
{
    if(m_ponlineUser->isHidden()){
        m_ponlineUser->show();
    }
    PDU* pdu=mkPDU(ENUM_MSG_TYPE_ONLINE_USER_REQUSET);
    Client::getInstance().sendMsg(pdu);
}

void Friend::on_flush_PB_clicked()
{
    flushFriend();
}

void Friend::on_del_PB_clicked()
{
    QListWidgetItem* pItem=ui->listWidget->currentItem();
    if(!pItem){
        QMessageBox::information(&Index::getInstance(),"删除好友","未选中好友");
        return;
    }
    QString tarName=pItem->text();
    int ret=QMessageBox::question(this,"删除好友",QString("是否确认删除好友'%1'?").arg(tarName));
    if(ret!=QMessageBox::Yes){
        return;
    }
    PDU* pdu=mkPDU(ENUM_MSG_TYPE_DELETE_FRIEND_REQUSET);
    memcpy(pdu->caData,Client::getInstance().loginName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,tarName.toStdString().c_str(),32);
    Client::getInstance().sendMsg(pdu);
}

void Friend::on_chat_PB_clicked()
{
    m_pChat->clearShowTE();
    QListWidgetItem* pItem=ui->listWidget->currentItem();
    if(!pItem){
        QMessageBox::information(&Index::getInstance(),"聊天","未选中好友");
        return;
    }
    m_pChat->strChatName=pItem->text();
    if(m_pChat->isHidden()){
        m_pChat->setWindowTitle(QString("to:%1").arg(m_pChat->strChatName));
        m_pChat->show();
    }
}
