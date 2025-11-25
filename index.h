#ifndef INDEX_H
#define INDEX_H

#include "file.h"
#include "friend.h"

#include <QWidget>

namespace Ui {
class Index;
}

class Index : public QWidget
{
    Q_OBJECT

public:    
    ~Index();
    static Index& getInstance();
    Friend* getFriend();
    File* getFile();
private slots:

    void on_friend_PB_clicked();

    void on_file_PB_clicked();

private:
    explicit Index(QWidget *parent = nullptr);
    Index(const Index& instance)=delete;
    Index& operator=(const Index&)=delete;
    Ui::Index *ui;
};

#endif // INDEX_H
