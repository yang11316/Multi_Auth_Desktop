#include "Login.h"
#include "./ui_login.h"

Login::Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
    connect(this->ui->btn_login, &QPushButton::clicked, this, &Login::slot_login_btn_clicked);
    connect(this->ui->btn_cancel, &QPushButton::clicked, this, &Login::slot_cancel_btn_clicked);
}

Login::~Login()
{
    delete ui;
}

void Login::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Return:
            slot_login_btn_clicked();
            break;
        case Qt::Key_Enter:
            slot_cancel_btn_clicked();
            break;
        default:
            break;
    }
}
void Login::slot_login_btn_clicked()
{
    this->qt_mainwindow.show();
    this->close();
}

/*
void Login::slot_login_btn_clicked()
{
    QString username = ui->edit_username_2->text();
    QString password = ui->edit_password_2->text();

    if (username.isEmpty() || password.isEmpty())
    {
        QMessageBox::warning(this, "警告", "用户名和口令不能为空");
    }
    else
    {
        int user_flag = QString::compare(username, this->user, Qt::CaseSensitive);
        int pass_flag = QString::compare(password, this->pass, Qt::CaseSensitive);


        if (!user_flag && !pass_flag)
        {
            //            mainwnd.show();
            this->close();
        }
        else
        {
            QMessageBox::information(this, "提示", "用户名或密码错误");
        }
    }
}
*/

void Login::slot_cancel_btn_clicked()
{
    this->close();

}

