#include "client.h"
#include "file.h"
#include "ui_file.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

File::File(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::File)
{
    ui->setupUi(this);
    m_strCurPath=QString("./filesys/%1").arg(Client::getInstance().loginName);
    m_strUserPath=m_strCurPath;
    m_pShareFile=new ShareFile;
    flushFile();
}

void File::updateFileList(QList<FileInfo *> pFileList)
{
    foreach(FileInfo* pFileInfo,m_pFileInfoList){
        delete pFileInfo;
    }
    m_pFileInfoList.clear();
    m_pFileInfoList=pFileList;
    ui->listWidget->clear();
    foreach(FileInfo* pFileInfo,pFileList){
        QListWidgetItem* pItem=new QListWidgetItem;
        if(pFileInfo->iFileType==0){
            pItem->setIcon(QIcon(QPixmap(":/dir.png")));
        }
        else{
            pItem->setIcon(QIcon(QPixmap(":/file.png")));
        }
        pItem->setText(pFileInfo->caName);
        ui->listWidget->addItem(pItem);
    }

}

void File::flushFile()
{
    PDU* pdu=mkPDU(ENUM_MSG_TYPE_FLUSH_FILE_REQUSET,m_strCurPath.toStdString().size()+1);
    memcpy(pdu->caMsg,m_strCurPath.toStdString().c_str(),m_strCurPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

void File::uploadFile()
{
    QFile file(m_strUploadFilePath);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this,"上传文件","打开文件失败");
        return;
    }
    PDU* pdu=mkPDU(ENUM_MSG_TYPE_UPLOAD_FILE_DATA_REQUSET, 4096);
    while(true){
        int ret = file.read(pdu->caMsg, 4096);
        if(ret == 0){
            break;
        }
        if(ret < 0){
           QMessageBox::warning(this,"上传文件","读取文件失败");
           break;
        }
        pdu->uiMsgLen = ret;
        pdu->uiPDULen = ret + sizeof(PDU);
        Client::getInstance().m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        qDebug()<< "send uiPDULen" << pdu->uiPDULen
                << "uiMsgLen" << pdu->uiMsgLen
                << "uiMsgType" << pdu->uiMsgType
                << "caData" << pdu->caData
                << "caData+32" << pdu->caData+32
                << "caMsg" << pdu->caMsg;
    }
    file.close();
    free(pdu);
    pdu=NULL;
}


File::~File()
{
    delete m_pShareFile;
    delete ui;
}

void File::on_mkdir_PB_clicked()
{
    QString strNewDir = QInputDialog::getText(this,"新建文件夹","新建文件夹名: ");
    if(strNewDir.isEmpty()||strNewDir.toStdString().size()>32){
        QMessageBox::information(this,"新建文件夹","文件夹名字长度非法");
        return;
    }
    PDU *pdu=mkPDU(ENUM_MSG_TYPE_MKDIR_REQUEST,m_strCurPath.toStdString().size()+1);
    memcpy(pdu->caData,strNewDir.toStdString().c_str(),32);
    memcpy(pdu->caMsg,m_strCurPath.toStdString().c_str(),m_strCurPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

void File::on_flush_PB_clicked()
{
    flushFile();
}

void File::on_deldir_PB_clicked()
{
    QListWidgetItem* pItem=ui->listWidget->currentItem();
    if(pItem==NULL){
        return;
    }
    QString strDelFileName = pItem->text();
    foreach(FileInfo* pFileInfo ,m_pFileInfoList){
        if(strDelFileName==pFileInfo->caName&&pFileInfo->iFileType!=0){
            QMessageBox::warning(this,"删除文件夹","选择的不是文件夹");
            return;
        }
    }
    int ret=QMessageBox::question(this,"删除文件夹",QString("是否删除文件夹 %1").arg(strDelFileName));
    if(ret!=QMessageBox::Yes){
        return;
    }
    QString strPath=QString("%1/%2").arg(m_strCurPath).arg(strDelFileName);
    PDU* pdu=mkPDU(ENUM_MSG_TYPE_DEL_DIR_REQUSET,strPath.toStdString().size()+1);
    memcpy(pdu->caMsg,strPath.toStdString().c_str(),strPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

void File::on_del_PB_clicked()
{
    QListWidgetItem* pItem=ui->listWidget->currentItem();
    if(pItem==NULL){
        return;
    }
    QString strDelFileName = pItem->text();
    foreach(FileInfo* pFileInfo ,m_pFileInfoList){
        if(strDelFileName==pFileInfo->caName&&pFileInfo->iFileType!=1){
            QMessageBox::warning(this,"删除文件","选择的不是文件");
            return;
        }
    }
    int ret=QMessageBox::question(this,"删除文件",QString("是否删除文件 %1").arg(strDelFileName));
    if(ret!=QMessageBox::Yes){
        return;
    }
    QString strPath=QString("%1/%2").arg(m_strCurPath).arg(strDelFileName);
    PDU* pdu=mkPDU(ENUM_MSG_TYPE_DEL_FILE_REQUSET,strPath.toStdString().size()+1);
    memcpy(pdu->caMsg,strPath.toStdString().c_str(),strPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

void File::on_rename_PB_clicked()
{
    QListWidgetItem* pItem=ui->listWidget->currentItem();
    if(pItem==NULL){
        return;
    }
    QString strOldName=pItem->text();
    QString strNewName=QInputDialog::getText(this,"重命名文件","新文件名");
    if(strNewName.isEmpty()||strNewName.toStdString().size()>32){
        QMessageBox::warning(this,"重命名文件","新文件名非法");
        return;
    }
    PDU* pdu=mkPDU(ENUM_MSG_TYPE_RENAME_FILE_REQUSET,m_strCurPath.toStdString().size()+1);
    memcpy(pdu->caData,strOldName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,strNewName.toStdString().c_str(),32);
    memcpy(pdu->caMsg,m_strCurPath.toStdString().c_str(),m_strCurPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}



void File::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString strDirName = item->text();
    foreach(FileInfo* pFileInfo,m_pFileInfoList){
        if(pFileInfo->caName==strDirName&&pFileInfo->iFileType!=0){
            QMessageBox::warning(this,"提示","选择的不是文件夹");
            return;
        }
    }
    m_strCurPath=QString("%1/%2").arg(m_strCurPath).arg(strDirName);
    flushFile();
}

void File::on_return_PB_clicked()
{
    if(m_strCurPath==m_strUserPath){
        QMessageBox::warning(this,"提示","不能再返回了");
        return;
    }
    int index=m_strCurPath.lastIndexOf('/');
    m_strCurPath.remove(index,m_strCurPath.toStdString().size()-index);
    flushFile();
}

void File::on_mv_PB_clicked()
{
    if(ui->mv_PB->text()=="移动文件"){
        QListWidgetItem* pItem=ui->listWidget->currentItem();
        if(pItem==NULL){
            return;
        }
        m_strMvFileName=pItem->text();
        m_strMvFilePath=QString("%1/%2").arg(m_strCurPath).arg(m_strMvFileName);
        QMessageBox::information(this,"移动文件","请选择要移动到的目录");
        ui->mv_PB->setText("确认/取消");
        return;
    }
    ui->mv_PB->setText("移动文件");
    QListWidgetItem* pItem=ui->listWidget->currentItem();
    QString strTarPath;
    if(pItem==NULL){
        strTarPath=QString("%1/%2").arg(m_strCurPath).arg(m_strMvFileName);
    }else{
        foreach(FileInfo* pFileInfo,m_pFileInfoList){
            if(pFileInfo->caName==pItem->text()&&pFileInfo->iFileType!=0){
                QMessageBox::warning(this,"提示","选择的不是文件夹");
                return;
            }
        }
        strTarPath=QString("%1/%2/%3").arg(m_strCurPath).arg(pItem->text()).arg(m_strMvFileName);

    }
    int ret = QMessageBox::information(this,"移动文件",QString("新的路径是否为%1").arg(strTarPath),"确认","取消");
    if(ret!=0){
        return;
    }
    int srcLen=m_strMvFilePath.toStdString().size();
    int tarLen=strTarPath.toStdString().size();
    PDU* pdu=mkPDU(ENUM_MSG_TYPE_MV_FILE_REQUSET,srcLen+tarLen+1);

    memcpy(pdu->caData,&srcLen,32);
    memcpy(pdu->caData+32,&tarLen,32);
    memcpy(pdu->caMsg,m_strMvFilePath.toStdString().c_str(),srcLen);
    memcpy(pdu->caMsg+srcLen,strTarPath.toStdString().c_str(),tarLen);
    Client::getInstance().sendMsg(pdu);
}

void File::on_share_PB_clicked()
{
    QListWidgetItem* pItem=ui->listWidget->currentItem();
    if(pItem==NULL) {
        QMessageBox::information(this,"分享文件","请选择要分享的文件");
        return;
    }
    m_pShareFile->m_strShareFilePath=m_strCurPath+'/'+pItem->text();
    m_pShareFile->updateFriend_LW();
    if(m_pShareFile->isHidden()){
        m_pShareFile->show();
    }

}

void File::on_upload_PB_clicked()
{
    m_strUploadFilePath.clear();
    m_strUploadFilePath=QFileDialog::getOpenFileName();
    qDebug() << m_strUploadFilePath;
    if(m_strUploadFilePath.isEmpty()){
        return;
    }
    PDU* pdu = mkPDU(ENUM_MSG_TYPE_UPLOAD_FILE_INIT_REQUSET,m_strUploadFilePath.toStdString().size()+1);
    int index = m_strUploadFilePath.lastIndexOf('/');
    QString strFileName=m_strUploadFilePath.right(m_strUploadFilePath.size()-index-1);
    memcpy(pdu->caData,strFileName.toStdString().c_str(),32);
    QFile file(m_strUploadFilePath);
    qint64 fileSize=file.size();
    memcpy(pdu->caData+32,&fileSize,sizeof(qint64));
    memcpy(pdu->caMsg,m_strCurPath.toStdString().c_str(),m_strUploadFilePath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

void File::on_download_PB_clicked()
{

}
