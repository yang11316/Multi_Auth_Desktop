#ifndef QT_FILEISSUANCE_H
#define QT_FILEISSUANCE_H

#include <QWidget>
#include<loader.h>
#include <QDebug>
#include <QCheckBox>
#include <QFileInfo>
#include <QDateTime>
#include <QMessageBox>

namespace Ui
{
    class QT_Fileissuance;
}

class QT_Fileissuance : public QWidget
{
    Q_OBJECT

public:
    explicit QT_Fileissuance(QWidget *parent = nullptr);
    ~QT_Fileissuance();


public slots:
    //处理client变化的情况
    void slot_change_client_from_mainwindow(QList<QString> client_list);

    //处理client_widget 点击的事件
    void slot_click_client_widget();
    //处理file_widget 点击的事件
    void slot_click_file_widget();

    //
    void slot_click_issuancebtn();

    void slot_checkbox_state_change(int);

signals:
    void signal_issuance_file_to_client(QString client_id, QList<QString> file_lsit);

private:
    Ui::QT_Fileissuance *ui;

    QMap<QString, QString> filemap;
    QMap<QString, QString> hostmap;
    QStringList itemlist;
    QSet<QString> existing_file;

    QString client_json_path = "./client_file.json";
    QString file_json_path = "./calculated_file.json";
};

#endif // QT_FILEISSUANCE_H
