#include "client.h"
#include "reshandler.h"
#include "uploader.h"

#include <QMessageBox>

ResHandler::ResHandler()
{

}

void ResHandler::regist()
{
    bool ret;
    memcpy(&ret,m_pdu->caData,sizeof(bool));
    if(ret){
        QMessageBox::information(&Client::getInstance(),"注册","注册成功");
    }
    else{
        QMessageBox::information(&Client::getInstance(),"注册","注册失败");
    }
}

void ResHandler::login(QString strLoginName)
{
    bool ret;
    memcpy(&ret,m_pdu->caData,sizeof(bool));
    if(ret){
        Index::getInstance().show();
        Index::getInstance().setWindowTitle(strLoginName);
        Index::getInstance().getFriend()->flushFriend();
        Client::getInstance().hide();
    }
    else
    {
        QMessageBox::information(&Client::getInstance(),"登录","登陆失败,该用户已经登录或检查用户名和密码");
    }
}

void ResHandler::findUser()
{
    qDebug()<<"findUser";
    char caName[32]={'\0'};
    int ret;
    memcpy(caName,m_pdu->caData,32);
    memcpy(&ret,m_pdu->caData+32,sizeof(int));
    if(ret==-1){
        QMessageBox::information(&Index::getInstance(),"查找用户","查找用户失败");
    }
    else if(ret==0){
        QMessageBox::information(&Index::getInstance(),"查找用户",QString("'%1' 不在线").arg(caName));
    }
    else if(ret==1){
        int ret=QMessageBox::information(&Index::getInstance(),"查找用户",QString("'%1' 在线").arg(caName),"添加好友","取消");
        if(ret==0){
            QString strCurName=Client::getInstance().loginName;
            QString strTarName=QString(caName);
            PDU* pdu=mkPDU(ENUM_MSG_TYPE_ADD_FRIEND_REQUSET);
            memcpy(pdu->caData,strCurName.toStdString().c_str(),32);
            memcpy(pdu->caData+32,strTarName.toStdString().c_str(),32);
            Client::getInstance().sendMsg(pdu);
        }
    }
    else if(ret==2){
        QMessageBox::information(&Index::getInstance(),"查找用户",QString("'%1' 不存在").arg(caName));
    }
}

void ResHandler::onlineUser(QString strLoginName)
{
    QStringList strList;
    char caTmp[32]={'\0'};
    uint uisize=m_pdu->uiMsgLen/32;
    for(uint i=0;i<uisize;i++){
        memcpy(caTmp,m_pdu->caMsg+i*32,32);
        if(caTmp==strLoginName){
            continue;
        }
        strList.append(caTmp);
    }
    Index::getInstance().getFriend()->m_ponlineUser->update_LW(strList);
}

void ResHandler::addFriend()
{
    int ret;
    memcpy(&ret,m_pdu->caData,sizeof(int));
    if(ret==-1){
        QMessageBox::information(&Index::getInstance(),"添加好友","添加好友错误");
    } else if(ret==0){
        QMessageBox::information(&Index::getInstance(),"添加好友","该用户不在线");
    } else if(ret==-2){
        QMessageBox::information(&Index::getInstance(),"添加好友","该用户已经是你的好友");
    }
}

void ResHandler::addFriendResend()
{
    char caName[32]={'\0'};
    memcpy(caName,m_pdu->caData,32);
    int ret=QMessageBox::question(&Index::getInstance(),"添加好友",QString("是否同意'%1'的添加好友请求?").arg(caName));
    if(ret!=QMessageBox::Yes){
        return;
    }
    PDU* respdu=mkPDU(ENUM_MSG_TYPE_ADD_FRIEND_AGREE_REQUSET);
    memcpy(respdu->caData,m_pdu->caData,64);
    Client::getInstance().sendMsg(respdu);
}

void ResHandler::addFriendAgree()
{
    bool ret;
    memcpy(&ret,m_pdu->caData,sizeof(bool));
    if(ret){
        QMessageBox::information(&Index::getInstance(),"添加好友","添加好友成功");
        Index::getInstance().getFriend()->flushFriend();
    } else {
        QMessageBox::information(&Index::getInstance(),"添加好友","添加好友失败");
    }
}

void ResHandler::flushFriend()
{
    qDebug()<<"flushFriend";
    QStringList strList;
    char caTmp[32]={'\0'};
    uint uisize=m_pdu->uiMsgLen/32;
    for(uint i=0;i<uisize;i++){
        memcpy(caTmp,m_pdu->caMsg+i*32,32);
        strList.append(caTmp);
    }
    Index::getInstance().getFriend()->update_LW(strList);
}

void ResHandler::deleteFriend()
{
    bool ret;
    memcpy(&ret,m_pdu->caData,sizeof(bool));
    if(!ret){
        QMessageBox::information(&Index::getInstance(),"删除好友","删除好友失败");
    }
    Index::getInstance().getFriend()->flushFriend();
}

void ResHandler::chat()
{
    Chat* c=Index::getInstance().getFriend()->m_pChat;
    if(c->isHidden()){
        c->show();
    }
    char strChatName[32]={'\0'};
    memcpy(strChatName,m_pdu->caData,32);
    c->strChatName=strChatName;
    char caChatName[32]={'\0'};
    memcpy(caChatName,m_pdu->caData,32);
    c->upDateShowTE(QString("%1:%2").arg(caChatName).arg(m_pdu->caMsg));
    c->setWindowTitle(QString("to:%1").arg(caChatName));
}

void ResHandler::mkdir()
{
    bool ret;
    memcpy(&ret,m_pdu->caData,sizeof(bool));
    if(ret){
        QMessageBox::information(&Index::getInstance(),"创建文件夹","创建成功");
        Index::getInstance().getFile()->flushFile();
    }
    else{
        QMessageBox::information(&Index::getInstance(),"创建文件夹","创建失败");
    }
}

void ResHandler::flushFile()
{
    int iCount=m_pdu->uiMsgLen/sizeof(FileInfo);
    qDebug()<< "file count"<<iCount;
    QList<FileInfo*> pFileList;
    for(int i=0;i<iCount;i++){
        FileInfo* pFileInfo=new FileInfo;
        memcpy(pFileInfo,m_pdu->caMsg+i*sizeof(FileInfo),sizeof(FileInfo));
        pFileList.append(pFileInfo);
    }
    Index::getInstance().getFile()->updateFileList(pFileList);
}

void ResHandler::delDir()
{
    bool ret;
    memcpy(&ret,m_pdu->caData,sizeof(bool));
    if(!ret){
        QMessageBox::information(&Index::getInstance(),"删除文件夹","删除失败");
    }
    else{
        QMessageBox::information(&Index::getInstance(),"删除文件夹","删除成功");
        Index::getInstance().getFile()->flushFile();
    }
}

void ResHandler::delFile()
{
    bool ret;
    memcpy(&ret,m_pdu->caData,sizeof(bool));
    if(!ret){
        QMessageBox::information(&Index::getInstance(),"删除文件","删除失败");
    }
    else{
        QMessageBox::information(&Index::getInstance(),"删除文件","删除成功");
        Index::getInstance().getFile()->flushFile();
    }
}

void ResHandler::renameFile()
{
    bool ret;
    memcpy(&ret,m_pdu->caData,sizeof(bool));
    if(!ret){
        QMessageBox::information(&Index::getInstance(),"重命名","重命名失败");
    }
    else{
        Index::getInstance().getFile()->flushFile();
    }
}

void ResHandler::mvFile()
{
    bool ret;
    memcpy(&ret,m_pdu->caData,sizeof(bool));
    if(!ret){
        QMessageBox::information(&Index::getInstance(),"移动","移动失败");
    }
    else{
        QMessageBox::information(&Index::getInstance(),"移动","移动成功");
        Index::getInstance().getFile()->flushFile();
    }
}

void ResHandler::shareFile()
{
    QMessageBox::information(&Index::getInstance(),"分享文件","文件已分享");
}

void ResHandler::shareFileResend()
{
    QString strFilePath=QString(m_pdu->caMsg);
    int index=strFilePath.lastIndexOf('/');
    QString strFileName=strFilePath.right(strFilePath.size()-index-1);

    int ret=QMessageBox::question(&Index::getInstance(),"分享文件",QString("%1分享文件%2,是否接受").arg(m_pdu->caData).arg(strFileName));
    if(ret!=QMessageBox::Yes){
        return;
    }
    PDU* pdu =mkPDU(ENUM_MSG_TYPE_SHARE_FILE_AGREE_REQUSET,m_pdu->uiMsgLen);
    memcpy(pdu->caData,Client::getInstance().loginName.toStdString().c_str(),32);
    memcpy(pdu->caMsg,m_pdu->caMsg,m_pdu->uiMsgLen);
    Client::getInstance().sendMsg(pdu);
}

void ResHandler::shareFileAgree()
{
    bool ret;
    memcpy(&ret,m_pdu->caData,sizeof(bool));
    if(!ret){
        QMessageBox::information(&Index::getInstance(),"接受分享","接收文件失败");
    }
    else {
        QMessageBox::information(&Index::getInstance(),"接受分享","接收文件成功");
        Index::getInstance().getFile()->flushFile();
    }
}

void ResHandler::uploadFileInit()
{
    bool ret;
    memcpy(&ret,m_pdu->caData,sizeof(bool));
    if(!ret){
        QMessageBox::information(&Index::getInstance(),"上传文件","上传文件初始化失败");
    }
    else{
        Client::getInstance().startUpload();
    }
}

void ResHandler::uploadFileData()
{
    bool ret;
    memcpy(&ret,m_pdu->caData,sizeof(bool));
    if(!ret){
        QMessageBox::information(&Index::getInstance(),"上传文件","上传文件读取失败");
    }
    else{
        QMessageBox::information(&Index::getInstance(),"上传文件","上传文件成功");
        Index::getInstance().getFile()->flushFile();
    }
}


