#include "server.h"
#include "ui_server.h"
#include "mytcpserver.h"

#include <QDebug>
#include <QFile>

Server::Server(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Server)
{
    ui->setupUi(this);
    loadconfig();
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP),m_usPort);
}

Server &Server::getInstance()
{
    static Server instance;
    return instance;
}

Server::~Server()
{
    delete ui;
}

void Server::loadconfig()
{
    QFile file(":/socket.config");
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray bydata = file.readAll();
        QString data = QString(bydata);
        QStringList strlist = data.split("\r\n");
        m_strIP = strlist.at(0);
        m_usPort = strlist.at(1).toUInt();
        m_strRootPath=strlist.at(2);
        qDebug() <<"loadconfig m_strIP"<< m_strIP
                 <<"m_usPort" << m_usPort
                 <<"m_strRootPath"<< m_strRootPath;
        file.close();
    }
    else
    {
        qDebug() <<"loadconfig failed";
    }
}


