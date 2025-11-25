#ifndef OPERATEDB_H
#define OPERATEDB_H

#include <QObject>
#include <QtSql/QSqlDatabase>
class OperateDB : public QObject
{
    Q_OBJECT
public:
    static OperateDB& getInstance();
    QSqlDatabase m_db;
    void conncet();
    bool handleRegist(const char* name,const char* pwd);
    bool handleLogin(const char* name,const char* pwd);
    void handleLine(const char* name);
    int handleFindUser(const char* name);
    QStringList handleOnlineUser();
    int addFriend(const char* carName,const char* tarName);
    bool handleAddFriendAgree(const char* curName,const char* tarName);
    QStringList handleFlushFriend(const char* curName);
    bool handleDelFriend(const char* curName,const char* tarName);
    ~OperateDB();
private:
    explicit OperateDB(QObject *parent = nullptr);
    OperateDB(const OperateDB& instance) =delete;
    OperateDB& operator=(const OperateDB&)=delete;
signals:

};

#endif // OPERATEDB_H
