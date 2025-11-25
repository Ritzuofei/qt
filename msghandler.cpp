#include "msghandler.h"
#include "operatedb.h"
#include <QDebug>
#include <QDir>
#include "mytcpserver.h"
#include "server.h"

MsgHandler::MsgHandler()
{

}

PDU *MsgHandler::regist()
{
    qDebug()<<"regist";
    char caName[32]={'\0'};
    char caPwd[32]={'\0'};
    memcpy(caName,m_pdu->caData,32);
    memcpy(caPwd,m_pdu->caData+32,32);
    qDebug() << "regist caName" << caName
             << "caPwd" << caPwd;
    bool ret=OperateDB::getInstance().handleRegist(caName,caPwd);
    qDebug() << "handleRegist ret" << ret;
    if(ret){
        QDir dir;
        dir.mkdir(QString("%1/%2").arg(Server::getInstance().m_strRootPath).arg(caName));
    }
    PDU* respdu=mkPDU(ENUM_MSG_TYPE_REGISTR_RESPOND);
    memcpy(respdu->caData,&ret,sizeof(bool));
    return respdu;
}

PDU *MsgHandler::login(QString& strname)
{
    qDebug()<<"login";
    char caName[32]={'\0'};
    char caPwd[32]={'\0'};
    memcpy(caName,m_pdu->caData,32);
    memcpy(caPwd,m_pdu->caData+32,32);
    qDebug()<< "login caName" << caName
            << "caPwd" << caPwd;
    bool ret=OperateDB::getInstance().handleLogin(caName,caPwd);
    qDebug() << "handleLogin ret" << ret;
    if(ret) strname=caName;
    PDU* respdu=mkPDU(ENUM_MSG_TYPE_LOGIN_RESPOND);
    memcpy(respdu->caData,&ret,sizeof(bool));
    return respdu;
}

PDU *MsgHandler::findUser()
{
    qDebug()<<"findUser";
    char caName[32]={'\0'};
    memcpy(caName,m_pdu->caData,32);
    qDebug()<< "findUser caName" << caName;
    int ret=OperateDB::getInstance().handleFindUser(caName);
    qDebug() << "handFindUser ret" << ret;
    PDU* respdu=mkPDU(ENUM_MSG_TYPE_FIND_USER_RESPOND);
    memcpy(respdu->caData,caName,sizeof(caName));
    memcpy(respdu->caData+32,&ret,sizeof(int));
    return respdu;
}

PDU *MsgHandler::onlineUser()
{
    qDebug()<<"onlineUser";
    QStringList strlist = OperateDB::getInstance().handleOnlineUser();
    uint sizes=strlist.size();
    PDU* respdu=mkPDU(ENUM_MSG_TYPE_ONLINE_USER_RESPOND,sizes*32);
    for(uint i=0;i<sizes;i++){
        memcpy(respdu->caMsg+i*32,strlist[i].toStdString().c_str(),32);
    }
    return respdu;
}

PDU *MsgHandler::addFriend()
{
    qDebug()<<"addFriend";
    char carName[32]={'\0'};
    char tarName[32]={'\0'};
    memcpy(carName,m_pdu->caData,32);
    memcpy(tarName,m_pdu->caData+32,32);
    qDebug()<< "carName" << carName;
    qDebug()<< "tarName" << tarName;
    int ret=OperateDB::getInstance().addFriend(carName,tarName);
    qDebug() << "addFriend ret" << ret;
    if(ret==1){
        MyTcpServer::getInstance().resend(tarName,m_pdu);
    }
    PDU* respdu=mkPDU(ENUM_MSG_TYPE_ADD_FRIEND_RESPOND);
    memcpy(respdu->caData,&ret,sizeof(int));
    return respdu;
}

PDU *MsgHandler::addFriendAgree()
{
    qDebug()<<"addFriendAgree";
    char curName[32]={'\0'};
    char tarName[32]={'\0'};
    memcpy(curName,m_pdu->caData,32);
    memcpy(tarName,m_pdu->caData+32,32);
    bool ret=OperateDB::getInstance().handleAddFriendAgree(curName,tarName);
    qDebug() << "addFriendAgree ret" << ret;
    PDU* respdu=mkPDU(ENUM_MSG_TYPE_ADD_FRIEND_AGREE_RESPOND);
    memcpy(respdu->caData,&ret,sizeof(bool));
    MyTcpServer::getInstance().resend(curName,respdu);
    return respdu;
}

PDU *MsgHandler::flushFriend()
{
    qDebug()<<"FlushFriend";
    char curName[32]={'\0'};
    memcpy(curName,m_pdu->caData,32);
    QStringList strList=OperateDB::getInstance().handleFlushFriend(curName);
    uint sizes=strList.size();
    PDU* respdu=mkPDU(ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND,sizes*32);
    for(uint i=0;i<sizes;i++){
        memcpy(respdu->caMsg+i*32,strList[i].toStdString().c_str(),32);
    }
    return respdu;
}

PDU *MsgHandler::delFriend()
{
    qDebug()<<"DeleteFriend";
    char curName[32]={'\0'};
    char tarName[32]={'\0'};
    memcpy(curName,m_pdu->caData,32);
    memcpy(tarName,m_pdu->caData+32,32);
    bool ret=OperateDB::getInstance().handleDelFriend(curName,tarName);
    PDU* respdu=mkPDU(ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND);
    memcpy(respdu->caData,&ret,sizeof(bool));
    if(ret){
        MyTcpServer::getInstance().resend(tarName,respdu);
    }
    return respdu;
}

PDU *MsgHandler::chat()
{
    char tarName[32]={'\0'};
    memcpy(tarName,m_pdu->caData+32,32);
    MyTcpServer::getInstance().resend(tarName,m_pdu);
    return NULL;
}

PDU *MsgHandler::mkdir()
{
    char dirName[32]={'\0'};
    memcpy(dirName,m_pdu->caData,32);
    QDir dir;
    bool ret=dir.mkdir(QString("%1/%2").arg(m_pdu->caMsg).arg(dirName));
    PDU* respdu=mkPDU(ENUM_MSG_TYPE_MKDIR_RESPOND);
    memcpy(respdu->caData,&ret,sizeof(bool));
    return respdu;
}

PDU *MsgHandler::flushFile()
{
    QDir dir(m_pdu->caMsg);
    QFileInfoList fileList=dir.entryInfoList();
    PDU* respdu=mkPDU(ENUM_MSG_TYPE_FLUSH_FILE_RESPOND,sizeof(FileInfo)*(fileList.size()-2));
    QString strFileName;
    FileInfo* pFileInfo;
    for(int i=0,j=0;i<fileList.size();i++){
        strFileName=fileList.at(i).fileName();
        if(strFileName==QString(".")||strFileName==QString("..")){
            continue;
        }
        pFileInfo=(FileInfo*)respdu->caMsg+j++;
        memcpy(pFileInfo->caName,strFileName.toStdString().c_str(),32);
        if(fileList.at(i).isDir()){
            pFileInfo->iFileType=0;
        }
        else{
            pFileInfo->iFileType=1;
        }
        qDebug() << "pFileInfo->caName" <<pFileInfo->caName
                 << "pFileInfo->iFileType" <<pFileInfo->iFileType;
    }
    return respdu;
}

PDU *MsgHandler::delDir()
{
    QFileInfo fileInfo(m_pdu->caMsg);
    bool ret=false;
    if(fileInfo.isDir()){
        QDir dir(m_pdu->caMsg);
        ret = dir.removeRecursively();
    }
    PDU* respdu=mkPDU(ENUM_MSG_TYPE_DEL_DIR_RESPOND);
    memcpy(respdu->caData,&ret,sizeof(bool));
    return respdu;
}

PDU *MsgHandler::delFile()
{
    QFileInfo fileInfo(m_pdu->caMsg);
    bool ret=false;
    if(fileInfo.isFile()){
        QDir dir;
        ret = dir.remove(m_pdu->caMsg);
    }
    PDU* respdu=mkPDU(ENUM_MSG_TYPE_DEL_FILE_RESPOND);
    memcpy(respdu->caData,&ret,sizeof(bool));
    return respdu;
}

PDU *MsgHandler::renameFile()
{
    char caOldName[32]={'\0'};
    char caNewName[32]={'\0'};
    memcpy(caOldName,m_pdu->caData,32);
    memcpy(caNewName,m_pdu->caData+32,32);

    QString strOldPath=QString("%1/%2").arg(m_pdu->caMsg).arg(caOldName);
    QString strNewPath=QString("%1/%2").arg(m_pdu->caMsg).arg(caNewName);
    PDU* respdu=mkPDU(ENUM_MSG_TYPE_RENAME_FILE_RESPOND);
    QDir dir;
    bool ret=dir.rename(strOldPath,strNewPath);
    memcpy(respdu->caData,&ret,sizeof(bool));
    return respdu;
}

PDU *MsgHandler::mvFlie()
{
    int oldLen=0;
    int newLen=0;
    memcpy(&oldLen,m_pdu->caData,sizeof(int));
    memcpy(&newLen,m_pdu->caData+32,sizeof(int));
    char* oldPath=new char[oldLen+1];
    char* newPath=new char[newLen+1];
    memset(oldPath,'\0',oldLen+1);
    memset(newPath,'\0',newLen+1);
    memcpy(oldPath,m_pdu->caMsg,oldLen);
    memcpy(newPath,m_pdu->caMsg+oldLen,newLen);
    qDebug() << "OldLen" << oldLen << "oldPath" << oldPath;
    qDebug() << "newLen" << newLen << "newPath" << newPath;
    QDir dir;
    bool ret = dir.rename(oldPath,newPath);
    PDU* respdu=mkPDU(ENUM_MSG_TYPE_MV_FILE_RESPOND);
    memcpy(respdu->caData,&ret,sizeof(bool));
    return respdu;
}

PDU *MsgHandler::shareFile()
{
    char curName[32]={'\0'};
    int iFriendNum=0;
    memcpy(curName,m_pdu->caData,32);
    memcpy(&iFriendNum,m_pdu->caData+32,sizeof(int));
    qDebug() << "m_pdu->caData" << m_pdu->caData
             << "curName" << curName;
    PDU* resendpdu=mkPDU(m_pdu->uiMsgType,m_pdu->uiMsgLen-iFriendNum*32);
    memcpy(resendpdu->caData,curName,32);
    memcpy(resendpdu->caMsg,m_pdu->caMsg+iFriendNum*32,m_pdu->uiMsgLen-iFriendNum*32);
    char caRecvName[32]={'\0'};
    for(int i=0;i<iFriendNum;i++){
        memcpy(caRecvName,m_pdu->caMsg+i*32,32);
        MyTcpServer::getInstance().resend(caRecvName,resendpdu);
    }
    free(resendpdu);
    resendpdu=NULL;
    PDU* respdu=mkPDU(ENUM_MSG_TYPE_SHARE_FILE_RESPOND);
    return respdu;
}

PDU *MsgHandler::shareFileAgree()
{
    QString strFilePath=QString(m_pdu->caMsg);
    int index=strFilePath.lastIndexOf('/');
    QString strFileName=strFilePath.right(strFilePath.size()-index-1);

    QString strRecvPath = QString("%1/%2/%3").arg(Server::getInstance().m_strRootPath,m_pdu->caData,strFileName);
    QFileInfo fileInfo(strFilePath);
    bool ret =true;
    if(fileInfo.isFile()){
        ret = QFile::copy(strFilePath,strRecvPath);
    } else {
        copyDir(strFilePath,strRecvPath);
    }
    PDU* respdu=mkPDU(ENUM_MSG_TYPE_SHARE_FILE_AGREE_RESPOND);
    memcpy(respdu->caData,&ret,sizeof(bool));
    return respdu;
}

bool MsgHandler::copyDir(QString strSrcDir, QString strDestDir)
{
    qDebug() << "strSrcDir" << strSrcDir
             << "strDestDir" <<strDestDir;
    QDir dir;
    bool ret = dir.mkdir(strDestDir);
    qDebug() << ret;
    dir.setPath(strSrcDir);

    QFileInfoList fList=dir.entryInfoList();
    QString srcPath;
    QString destPath;
    for(int i=0;i<fList.size();i++){
        if(fList[i].fileName()==QString(".")||fList[i].fileName()==QString("..")){
            continue;
        }
        srcPath=strSrcDir + '/' + fList[i].fileName();
        destPath=strDestDir + '/' + fList[i].fileName();
        if(fList[i].isFile()){
            ret = QFile::copy(srcPath,destPath);
        }
        else{
            ret = copyDir(srcPath,destPath);
        }
        qDebug() << ret << fList[i].fileName();
    }
    return ret;
}

PDU *MsgHandler::uploadFileInit()
{
    char caFileName[32]={'\0'};
    memcpy(caFileName,m_pdu->caData,32);
    m_iUploadFileSize = 0;
    memcpy(&m_iUploadFileSize,m_pdu->caData+32,sizeof(qint64));
    QString strPath = QString("%1/%2").arg(m_pdu->caMsg).arg(caFileName);
    m_fUploadFile.setFileName(strPath);
    m_iReceiveSize=0;
    bool ret = m_fUploadFile.open(QIODevice::WriteOnly);
    PDU* respdu=mkPDU(ENUM_MSG_TYPE_UPLOAD_FILE_INIT_RESPOND);
    memcpy(respdu->caData,&ret,sizeof(bool));
    return respdu;
}

PDU *MsgHandler::uploadFileData()
{
    m_fUploadFile.write(m_pdu->caMsg,m_pdu->uiMsgLen);
    m_iReceiveSize+=m_pdu->uiMsgLen;
    if(m_iReceiveSize < m_iUploadFileSize){
        return NULL;
    }
    m_fUploadFile.close();
    PDU* respdu=mkPDU(ENUM_MSG_TYPE_UPLOAD_FILE_DATA_RESPOND);
    bool ret= m_iReceiveSize==m_iUploadFileSize;
    memcpy(respdu->caData,&ret,sizeof(bool));
    return respdu;
}




















