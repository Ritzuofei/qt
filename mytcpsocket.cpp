#include "mytcpserver.h"
#include "mytcpsocket.h"
#include "operatedb.h"
#include "protocol.h"

MyTcpSocket::MyTcpSocket()
{

    m_pmh=new MsgHandler;
}

void MyTcpSocket::sendMsg(PDU *pdu)
{
    if(pdu == NULL){
        return;
    }
    this->write((char*)pdu,pdu->uiPDULen);
    qDebug()<< "send uiPDULen" << pdu->uiPDULen
            << "uiMsgLen" << pdu->uiMsgLen
            << "uiMsgType" << pdu->uiMsgType
            << "caData" << pdu->caData
            << "caData+32" << pdu->caData+32
            << "caMsg" << pdu->caMsg;
    free(pdu);
    pdu=NULL;
}


PDU *MyTcpSocket::handlePDU(PDU *pdu)
{
    m_pmh->m_pdu=pdu;
    qDebug()<< "handlePDU uiPDULen" << pdu->uiPDULen
            << "uiMsgLen" << pdu->uiMsgLen
            << "uiMsgType" << pdu->uiMsgType
            << "caData" << pdu->caData
            << "caData+32" << pdu->caData+32
            << "caMsg" << pdu->caMsg;
    switch(pdu->uiMsgType){
    case ENUM_MSG_TYPE_REGISTR_REQUSET:{
        return m_pmh->regist();
    }
    case ENUM_MSG_TYPE_LOGIN_REQUSET:{
        return m_pmh->login(loginName);
    }
    case ENUM_MSG_TYPE_FIND_USER_REQUSET:{
        return m_pmh->findUser();
    }
    case ENUM_MSG_TYPE_ONLINE_USER_REQUSET:{
        return m_pmh->onlineUser();
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUSET:{
        return m_pmh->addFriend();
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGREE_REQUSET:{
        return m_pmh->addFriendAgree();
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUSET:{
        return m_pmh->flushFriend();
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUSET:{
        return m_pmh->delFriend();
    }
    case ENUM_MSG_TYPE_CHAT_REQUSET:{
        return m_pmh->chat();
    }
    case ENUM_MSG_TYPE_MKDIR_REQUEST:{
        return m_pmh->mkdir();
    }
    case ENUM_MSG_TYPE_FLUSH_FILE_REQUSET:{
        return m_pmh->flushFile();
    }
    case ENUM_MSG_TYPE_DEL_DIR_REQUSET:{
        return m_pmh->delDir();
    }
    case ENUM_MSG_TYPE_DEL_FILE_REQUSET:{
        return m_pmh->delFile();
    }
    case ENUM_MSG_TYPE_RENAME_FILE_REQUSET:{
        return m_pmh->renameFile();
    }
    case ENUM_MSG_TYPE_MV_FILE_REQUSET:{
        return m_pmh->mvFlie();
    }
    case ENUM_MSG_TYPE_SHARE_FILE_REQUSET:{
        return m_pmh->shareFile();
    }
    case ENUM_MSG_TYPE_SHARE_FILE_AGREE_REQUSET:{
        return m_pmh->shareFileAgree();
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_INIT_REQUSET:{
        return m_pmh->uploadFileInit();
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_DATA_REQUSET:{
        return m_pmh->uploadFileData();
    }
    default:
        break;
    }
    return NULL;
}

MyTcpSocket::~MyTcpSocket()
{
    delete m_pmh;
}

void MyTcpSocket::recvMsg()
{
    qDebug()<<"recvMsg 接受到消息的长度"<<this->bytesAvailable();
    QByteArray data =this->readAll();
    buffer.append(data);
    while(buffer.size()>=int(sizeof(PDU))){
        PDU* pdu=(PDU*)buffer.data();
        if(buffer.size()<int(pdu->uiPDULen)){
            break;
        }
        PDU* respdu=handlePDU(pdu);
        sendMsg(respdu);
        buffer.remove(0,int(pdu->uiPDULen));
    }
}

void MyTcpSocket::userLine()
{
    OperateDB::getInstance().handleLine(loginName.toStdString().c_str());
    MyTcpServer::getInstance().deleteSocket(this);
}
