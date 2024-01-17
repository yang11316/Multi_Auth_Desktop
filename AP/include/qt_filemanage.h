#ifndef QT_FILEMANAGE_H
#define QT_FILEMANAGE_H

#include <QWidget>
#include <QMap>
#include <QSet>
#include <QFileInfo>
#include<QDateTime>

#include <loader.h>

namespace Ui
{
    class QT_FileManage;
}

class QT_FileManage : public QWidget
{
    Q_OBJECT

public:
    explicit QT_FileManage(QWidget *parent = nullptr);
    ~QT_FileManage();


public slots:
    void slot_update_filewidget();

    void slot_filewidget_clicked();

    void slot_keybtn_clicked();


signals:
    void signal_cal_file_fullkey_to_mainwindow(QString filepath);


private:
    Ui::QT_FileManage *ui;
    QString save_file_path = "calculated_file.json";

    QMap<QString, QString> pid_map;
    QMap<QString, QString> partical1_map;
    QMap<QString, QString> partical2_map;

    QSet<QString> existing_file;


};

#endif // QT_FILEMANAGE_H
