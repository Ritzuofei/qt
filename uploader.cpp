#include "protocol.h"
#include "uploader.h"

#include <QFile>
#include <QThread>

Uploader::Uploader()
{

}

Uploader::Uploader(QString strPath)
{
    m_strUploadFilePath=strPath;
}

void Uploader::uploadFile()
{
    QFile file(m_strUploadFilePath);
    if(!file.open(QIODevice::ReadOnly)){
        emit handlError("打开文件失败");
        emit finished();
        return;
    }
    while(true){
        PDU* pdu=mkPDU(ENUM_MSG_TYPE_UPLOAD_FILE_DATA_REQUSET, 40960);
        int ret = file.read(pdu->caMsg, 40960);
        if(ret == 0){
            break;
        }
        if(ret < 0){
            emit handlError("读取文件失败");
           break;
        }
        pdu->uiMsgLen = ret;
        pdu->uiPDULen = ret + sizeof(PDU);
        emit uploadPDU(pdu);
    }
    file.close();
    emit finished();
}

void Uploader::start()
{
    QThread* thread=new QThread;
    this->moveToThread(thread);
    connect(thread,&QThread::started,this,&Uploader::uploadFile);
    connect(this,&Uploader::finished,thread,&QThread::quit);
    connect(thread,&QThread::finished,thread,&QThread::deleteLater);
    thread->start();

}
