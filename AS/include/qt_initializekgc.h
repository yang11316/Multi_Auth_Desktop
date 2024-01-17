#ifndef QT_INITIALIZEKGC_H
#define QT_INITIALIZEKGC_H

#include <QWidget>

namespace Ui
{
    class QT_InitializeKGC;
}


//==================================================================
//类 名：  QT_InitializeKGC
//功    能： kgc初始化
//日    期： 2024/1/12
//版    本： 1.0
//==================================================================
class QT_InitializeKGC : public QWidget
{
    Q_OBJECT

public:
    explicit QT_InitializeKGC(QWidget *parent = nullptr);
    ~QT_InitializeKGC();


signals:
    void signal_initialbtn_to_mainwindow();
    void signal_resetkgc_to_mainwindow();

public slots:
    void slot_startbtn_clicked();
    void slot_resetbtn_clicked();
    void slot_receive_acc_kgc_initial_from_mainwindow();

    void slot_receive_message_from_mainwindow(QString str);

private:
    Ui::QT_InitializeKGC *ui;
};

#endif // QT_INITIALIZEKGC_H
