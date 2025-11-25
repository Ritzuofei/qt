#include "operatedb.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

OperateDB &OperateDB::getInstance()
{
    static OperateDB instance;
    return instance;
}

void OperateDB::conncet()
{
    m_db.setHostName("localhost");
    m_db.setPort(3306);
    m_db.setUserName("root");
    m_db.setPassword("123456");
    m_db.setDatabaseName("qt");
    if(m_db.open()){
       qDebug() <<"数据库连接成功";
    } else {
       qDebug() <<"数据库连接失败" << m_db.lastError().text();
    }
}

bool OperateDB::handleRegist(const char *name, const char *pwd)
{
    if(name==NULL||pwd==NULL){
        return false;
    }
    QString sql=QString("select * from user_info where name ='%1'").arg(name);
    qDebug()<<"handleRegist 查找用户sql"<<sql;
    QSqlQuery q;
    bool ret=q.exec(sql);
    if(!ret){
        return false;
    }
    if(q.next()){
        return false;
    }

    sql=QString("insert into user_info(name,pwd) values('%1','%2')").arg(name).arg(pwd);
    qDebug() <<"handleRegist插入一个用户sql"<<sql;
    return q.exec(sql);
}

bool OperateDB::handleLogin(const char *name, const char *pwd)
{
    if(name==NULL||pwd==NULL){
        return false;
    }
    QString sql=QString("select * from user_info where name = '%1' AND pwd= '%2'").arg(name).arg(pwd);
    qDebug() << "handleLogin 查找用户sql" << sql;
    QSqlQuery q;
    bool ret=q.exec(sql);
    if(!ret){
        return false;
    }
    if(!q.next()){
        return false;
    }
    qDebug()<< "handleLogin 用户数据 id" << q.value(0).toString()
            << "name" << q.value(1).toString()
            << "pwq" << q.value(2).toString()
            << "online" << q.value(3).toString();
    if(q.value(3)==1){
        return false;
    }
    sql=QString("update user_info set online = 1 where name = '%1' AND pwd= '%2' ").arg(name).arg(pwd);
    qDebug() << "handleLogin 修改用户online状态sql" << sql;
    return q.exec(sql);
}

void OperateDB::handleLine(const char *name)
{
    if(name==NULL){
        return;
    }
    QString sql=QString("update user_info set online = 0 where name = '%1'").arg(name);
    QSqlQuery q;
    q.exec(sql);
}

int OperateDB::handleFindUser(const char *name)
{
    if(name==NULL){
        return -1;
    }
    QString sql=QString("select online from user_info where name = '%1'").arg(name);
    qDebug()<< "handleFindUser 查找用户sql"<<sql;
    QSqlQuery q;
    if(!q.exec(sql)){
        return -1;
    }
    else if(q.next()){
        qDebug() << "用户online" <<q.value(0).toString().toUInt();
        return q.value(0).toString().toUInt();
    }
    return 2;
}

QStringList OperateDB::handleOnlineUser()
{
    QString sql=QString("select name from user_info where online = 1");
    QSqlQuery q;
    q.exec(sql);
    QStringList list;
    while(q.next())
    {
        list.append(q.value(0).toString());
    }
    return list;
}

int OperateDB::addFriend(const char *curName, const char *tarName)
{
    if(curName==NULL||tarName==NULL) {
        return -1;
    }
    QString data=QString(R"(
            select * from friend where
            (
                user_id = (select id from user_info where name = '%1')
                and
                friend_id = (select id from user_info where name = '%2')
                or
                user_id = (select id from user_info where name = '%2')
                and
                friend_id = (select id from user_info where name = '%1')
            )
    )").arg(curName).arg(tarName);
    qDebug() << "handleAddFriend" << data;
    QSqlQuery q;
    q.exec(data);
    if(q.next()){
        return -2;//已经是好友
    }
    data=QString("select online from user_info where name='%1'").arg(tarName);
    q.exec(data);
    if(q.next()){
        return q.value(0).toInt();//0不在线，1在线
    }
    return -1;
}


bool OperateDB::handleAddFriendAgree(const char *curName, const char *tarName)
{
    qDebug() << "handleAddFriendAgree";
    if(curName==NULL||tarName==NULL) {
        return false;
    }
    QString data=QString(R"(
            insert into friend(user_id,friend_id)
            select u1.id,u2.id from user_info u1,user_info u2
            where u1.name='%1' and u2.name ='%2'
    )").arg(curName).arg(tarName);
    qDebug() <<"handleAddFriendAgree sql\n"<<data;
    QSqlQuery q;
    return q.exec(data);
}

QStringList OperateDB::handleFlushFriend(const char *curName)
{
    QStringList list;
    qDebug() << "handleFlushFriend";
    QString sql=QString(R"(select name from user_info where id in
                        (
                        select user_id from friend where friend_id=
                          (select id from user_info where name ='%1')
                        union
                        select friend_id from friend where user_id=
                          (select id from user_info where name ='%1')
                        ) and online = 1)").arg(curName);
    QSqlQuery q;
    q.exec(sql);
    qDebug() << "sql" << sql;
    while(q.next()){
        list.append(q.value(0).toString());
    }
    return list;
}

bool OperateDB::handleDelFriend(const char *curName, const char *tarName)
{
    if(curName==NULL||tarName==NULL){
        return false;
    }
    QString data=QString(R"(
            select * from friend where
                (
                 user_id=(select id from user_info where name='%1')
                 and
                 friend_id=(select id from user_info where name='%2')
                )
                or
                (
                 user_id=(select id from user_info where name='%2')
                 and
                 friend_id=(select id from user_info where name='%1')
                )
                         )").arg(curName,tarName);
    qDebug() << "handleDelFriend query friend"<<data;
    QSqlQuery q;
    q.exec(data);
    if(!q.next()){
        return false;
    }
    data=QString(R"(
                 delete from friend where
                     (
                      user_id=(select id from user_info where name='%1')
                      and
                      friend_id=(select id from user_info where name='%2')
                     )
                     or
                     (
                      user_id=(select id from user_info where name='%2')
                      and
                      friend_id=(select id from user_info where name='%1')
                     )
                              )").arg(curName,tarName);
    qDebug() << "handleDelFriend delete friend"<<data;
    return q.exec(data);
}

OperateDB::~OperateDB()
{
    m_db.close();
}

OperateDB::OperateDB(QObject *parent) : QObject(parent)
{
    m_db=QSqlDatabase::addDatabase("QMYSQL");
}
