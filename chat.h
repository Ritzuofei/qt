#ifndef CHAT_H
#define CHAT_H

#include "protocol.h"

#include <QWidget>

namespace Ui {
class Chat;
}

class Chat : public QWidget
{
    Q_OBJECT

public:
    QString strChatName;
    void upDateShowTE(QString strMSG);
    void clearShowTE();
    explicit Chat(QWidget *parent = nullptr);
    ~Chat();

private slots:
    void on_send_PB_clicked();

private:
    Ui::Chat *ui;
};

#endif // CHAT_H
