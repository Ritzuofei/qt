#ifndef RESHANDLER_H
#define RESHANDLER_H

#include "protocol.h"

#include <QString>



class ResHandler
{
public:
    ResHandler();
    PDU* m_pdu;
    void regist();
    void login(QString strLoginName);
    void findUser();
    void onlineUser(QString strLoginName);
    void addFriend();
    void addFriendResend();
    void addFriendAgree();
    void flushFriend();
    void deleteFriend();
    void chat();
    void mkdir();
    void flushFile();
    void delDir();
    void delFile();
    void renameFile();
    void mvFile();
    void shareFile();
    void shareFileResend();
    void shareFileAgree();
    void uploadFileInit();
    void uploadFileData();

};

#endif // RESHANDLER_H
