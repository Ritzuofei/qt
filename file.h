#ifndef FILE_H
#define FILE_H

#include "protocol.h"
#include "sharefile.h"

#include <QListWidget>
#include <QWidget>

namespace Ui {
class File;
}

class File : public QWidget
{
    Q_OBJECT

public:
    QString m_strCurPath;
    QString m_strUserPath;
    QString m_strMvFileName;
    QString m_strMvFilePath;
    QList<FileInfo* > m_pFileInfoList;
    ShareFile* m_pShareFile;
    QString m_strUploadFilePath;
    QString m_strDownloadFilePath;
    explicit File(QWidget *parent = nullptr);
    void updateFileList(QList<FileInfo*> pFileList);
    void flushFile();
    void uploadFile();
    ~File();

private slots:
    void on_mkdir_PB_clicked();

    void on_flush_PB_clicked();


    void on_deldir_PB_clicked();

    void on_del_PB_clicked();

    void on_rename_PB_clicked();


    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_return_PB_clicked();

    void on_mv_PB_clicked();

    void on_share_PB_clicked();

    void on_upload_PB_clicked();

    void on_download_PB_clicked();

private:
    Ui::File *ui;
};

#endif // FILE_H
