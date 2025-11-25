#ifndef FRIEND_H
#define FRIEND_H

#include "onlineuser.h"
#include "chat.h"

#include <QWidget>

namespace Ui {
class Friend;
}

class Friend : public QWidget
{
    Q_OBJECT

public:
    explicit Friend(QWidget *parent = nullptr);
    OnlineUser* m_ponlineUser;
    Chat* m_pChat;
    void update_LW(QStringList list);
    void flushFriend();
    QListWidget* getFriend_LW();
    ~Friend();

private slots:
    void on_findUser_PB_clicked();

    void on_onlineUser_PB_clicked();

    void on_flush_PB_clicked();

    void on_del_PB_clicked();

    void on_chat_PB_clicked();

private:
    Ui::Friend *ui;
};

#endif // FRIEND_H
