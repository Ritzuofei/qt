#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include "protocol.h"
#include "msghandler.h"

#include <QObject>
#include <QTcpSocket>

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
    void sendMsg(PDU* pdu);
    PDU* handlePDU(PDU* pdu);
    MsgHandler* m_pmh;
    QString loginName;
    QByteArray buffer;
    ~MyTcpSocket();
public :
    void recvMsg();
    void userLine();
};

#endif // MYTCPSOCKET_H
