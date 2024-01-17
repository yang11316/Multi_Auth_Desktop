#ifndef WIDGET_H
#define WIDGET_H

#pragma execution_character_set("utf-8")
#include <QWidget>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPushButton>

#include "qt_mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class Login;
}
QT_END_NAMESPACE

//==================================================================
//类 名：  Login
//功    能： 进行登陆验证
//日    期： 2024/1/12
//版    本： 1.0
//==================================================================

class Login : public QWidget
{
    Q_OBJECT

public:
    Login(QWidget *parent = nullptr);
    ~Login();
protected:
    //对回车进行响应
    virtual void keyPressEvent(QKeyEvent *event);


private slots:
    void slot_login_btn_clicked();
    void slot_cancel_btn_clicked();


private:
    Ui::Login *ui;
    QT_MainWindow qt_mainwindow;
    QString user = "admin";
    QString pass = "admin";
};
#endif // WIDGET_H
