#include "server.h"
#include "operatedb.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OperateDB::getInstance().conncet();
    Server::getInstance().show();
    return a.exec();
}
