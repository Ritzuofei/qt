#include "client.h"
#include "index.h"
#include "protocol.h"
#include "ui_client.h"
#include "uploader.h"

#include <QFile>
#include <QDebug>
#include <QHostAddress>
#include <QMessageBox>

Client::Client(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Client)
{
    ui->setupUi(this);
    loadconfig();
    connect(&m_tcpSocket,&QTcpSocket::connected,this,&Client::showConnect);
    connect(&m_tcpSocket,&QTcpSocket::readyRead,this,&Client::recvMsg);
    m_tcpSocket.connectToHost(QHostAddress(m_strIP),m_usPort);
    m_prh=new ResHandler;
}

void Client::loadconfig()
{
    QFile file(":/socket.config");
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray baData=file.readAll();
        QString data=QString(baData);
        QStringList strList = data.split("\r\n");
        m_strIP = strList.at(0);
        m_usPort = strList.at(1).toUShort();
        qDebug() << "loadConfig m_strIP" << m_strIP
                 << "m_usPort"<<m_usPort;
        file.close();
    }
    else
    {
        qDebug() << "loadconfig failed";
    }
}

Client::~Client()
{
    delete ui;
    delete m_prh;
}

Client &Client::getInstance()
{
    static Client instance;
    return instance;
}

void Client::sendMsg(PDU *pdu)
{
    m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
    qDebug()<< "send uiPDULen" << pdu->uiPDULen
            << "uiMsgLen" << pdu->uiMsgLen
            << "uiMsgType" << pdu->uiMsgType
            << "caData" << pdu->caData
            << "caData+32" << pdu->caData+32
            << "caMsg" << pdu->caMsg;
    free(pdu);
    pdu=NULL;
}

void Client::handlePDU(PDU *pdu)
{
    m_prh->m_pdu=pdu;
    qDebug()<< "receive uiPDULen" << pdu->uiPDULen
            << "uiMsgLen" << pdu->uiMsgLen
            << "uiMsgType" << pdu->uiMsgType
            << "caData" << pdu->caData
            << "caData+32" << pdu->caData+32
            << "caMsg" << pdu->caMsg;
    switch(pdu->uiMsgType){
    case ENUM_MSG_TYPE_REGISTR_RESPOND:{
        m_prh->regist();
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_RESPOND:{
        m_prh->login(loginName);
        break;
    }
    case ENUM_MSG_TYPE_FIND_USER_RESPOND:{
        m_prh->findUser();
        break;
    }
    case ENUM_MSG_TYPE_ONLINE_USER_RESPOND:{
        m_prh->onlineUser(loginName);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:{
        m_prh->addFriend();
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUSET:{
        m_prh->addFriendResend();
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGREE_RESPOND:{
        m_prh->addFriendAgree();
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:{
        m_prh->flushFriend();
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND:{
        m_prh->deleteFriend();
        break;
    }
    case ENUM_MSG_TYPE_CHAT_REQUSET:{
        m_prh->chat();
        break;
    }
    case ENUM_MSG_TYPE_MKDIR_RESPOND:{
        m_prh->mkdir();
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FILE_RESPOND:{
        m_prh->flushFile();
        break;
    }
    case ENUM_MSG_TYPE_DEL_DIR_RESPOND:{
        m_prh->delDir();
        break;
    }
    case ENUM_MSG_TYPE_DEL_FILE_RESPOND:{
        m_prh->delFile();
        break;
    }
    case ENUM_MSG_TYPE_RENAME_FILE_RESPOND:{
        m_prh->renameFile();
        break;
    }
    case ENUM_MSG_TYPE_MV_FILE_RESPOND:{
        m_prh->mvFile();
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_RESPOND:{
        m_prh->shareFile();
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_REQUSET:{
        m_prh->shareFileResend();
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_AGREE_RESPOND:{
        m_prh->shareFileAgree();
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_INIT_RESPOND:{
        m_prh->uploadFileInit();
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_DATA_RESPOND:{
        m_prh->uploadFileData();
        break;
    }
    default:
        break;
    }
}

void Client::startUpload()
{
    Uploader* uploader=new Uploader(Index::getInstance().getFile()->m_strUploadFilePath);
    connect(uploader,&Uploader::handlError,this,&Client::uploadError);
    connect(uploader,&Uploader::uploadPDU,this,&Client::sendMsg);
    uploader->start();
}


void Client::showConnect()
{
    qDebug() << "连接服务器成功";
}

void Client::recvMsg()
{
    qDebug()<<"recvMsg 接受到消息的长度"<<m_tcpSocket.bytesAvailable();
    QByteArray data =m_tcpSocket.readAll();
    buffer.append(data);
    while(buffer.size()>=int(sizeof(PDU))){
        PDU* pdu=(PDU*)buffer.data();
        if(buffer.size()<int(pdu->uiPDULen)){
            break;
        }
        handlePDU(pdu);
        buffer.remove(0,int(pdu->uiPDULen));
    }
}

void Client::uploadError(const QString &error)
{
    QMessageBox::information(&Index::getInstance(),"上传文件",error);
}

void Client::on_regist_PB_clicked()
{
    QString strName=ui->name_LE->text();
    QString strPwd=ui->pwd_LE->text();
    if(strName.toStdString().size()>32||strPwd.toStdString().size()>32||strName.isEmpty()||strPwd.isEmpty())
    {
        QMessageBox::information(this,"注册","用户名或密码长度非法");
        return;
    }
    PDU* pdu=mkPDU(ENUM_MSG_TYPE_REGISTR_REQUSET);
    memcpy(pdu->caData,strName.toStdString().c_str(),strName.toStdString().size());
    memcpy(pdu->caData+32,strPwd.toStdString().c_str(),sizeof(strPwd));
    sendMsg(pdu);
}

void Client::on_login_PB_clicked()
{
    QString strName=ui->name_LE->text();
    QString strPwd=ui->pwd_LE->text();
    loginName=strName;
    if(strName.toStdString().size()>32||strPwd.toStdString().size()>32||strName.isEmpty()||strPwd.isEmpty())
    {
        QMessageBox::information(this,"登录","用户名或密码长度非法");
        return;
    }

    PDU* pdu=mkPDU(ENUM_MSG_TYPE_LOGIN_REQUSET);
    memcpy(pdu->caData,strName.toStdString().c_str(),strName.toStdString().size());
    memcpy(pdu->caData+32,strPwd.toStdString().c_str(),sizeof(strPwd));
    sendMsg(pdu);
}
