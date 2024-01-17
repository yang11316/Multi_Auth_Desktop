#ifndef QT_FILECHOOSE_H
#define QT_FILECHOOSE_H

#include "loader.h"
#include <QWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QPushButton>
#include <QFileInfo>
#include <QDebug>
#include <QDateTime>
#include <QDropEvent>
#include <QMimeData>
#include <QCheckBox>
#include <list>



namespace Ui
{
    class QT_FileChoose;
}

//==================================================================
//类 名：  QT_FileChoose
//功    能： 选择文件并计算pid值，保存至本地json
//日    期： 2024/1/13
//版    本： 1.0
//==================================================================

class QT_FileChoose : public QWidget
{
    Q_OBJECT

public:
    explicit QT_FileChoose(QWidget *parent = nullptr);
    ~QT_FileChoose();
    void check_file_form_json();

    QString get_file_pid(QString filepath);

    QString get_file_hash(QString filepath);

public slots:
    //选择文件按钮触发
    void slot_filechosebtn_clicked();

    //按钮框状态改变（框选、锁定、空白）
    void slot_checkboxstate_changed(int);

    //listwidget控件点击事件
    void slot_listwidget_clicked();

    //删除按钮触发
    void slot_fliedeltebtn_clicked();

    //撤销按钮触发事件
    void slot_clearbtn_clicked();

    //计算按钮触发
    void slot_calculatebtn_clicked();

    void slot_check_box_state_changed(int);

    void slot_update_filemap(QMap<QString, QString> sendmap);


signals:
    void signal_calculate_file_to_mainwindow(QStringList itemlist);

private:
    Ui::QT_FileChoose *ui;

    //filemap:<filepath ,filename>
    QMap<QString, QString> filemap;

    //存储已授权的文件
    QSet<QString> existing_file;

    QStringList itemlist;

    QString savepath = "./calculated_file.json";
};

#endif // QT_FILECHOOSE_H
