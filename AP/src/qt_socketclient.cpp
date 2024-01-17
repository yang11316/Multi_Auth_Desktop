#include "qt_socketclient.h"
#include "ui_qt_socketclient.h"

QT_SocketClient::QT_SocketClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QT_SocketClient)
{
    ui->setupUi(this);
    connect(this->ui->btn_connect, &QPushButton::clicked, this, &QT_SocketClient::slot_connectbtn_clicked);
    connect(this->ui->btn_disconnect, &QPushButton::clicked, this, &QT_SocketClient::slot_disconncet_clicked);
    connect(this->ui->btn_send, &QPushButton::clicked, this, &QT_SocketClient::slot_sendbtn_clicked);

}

QT_SocketClient::~QT_SocketClient()
{
    delete ui;
}

void QT_SocketClient::slot_connectbtn_clicked()
{
    QString ip = this->ui->edit_ip->text();
    quint16 port = this->ui->edit_port->value();
    emit signal_connect_to_server(ip, port);
    this->ui->btn_connect->setEnabled(false);
    this->ui->btn_disconnect->setEnabled(true);
}

void QT_SocketClient::slot_disconncet_clicked()
{
    emit signal_disconnect_to_server();
    this->ui->btn_connect->setEnabled(true);
    this->ui->btn_disconnect->setEnabled(false);
}

void QT_SocketClient::slot_sendbtn_clicked()
{
    if (!this->ui->edit_sendmsg->document()->isEmpty())
    {
        QString msg = this->ui->edit_sendmsg->toPlainText();
        signal_send_msg_to_server('0', msg);
    }

}

void QT_SocketClient::slot_receive_connect_from_mainwindow()
{
    this->ui->edit_recvmsg->insertPlainText("[server]connect\n");
}

void QT_SocketClient::slot_receive_commonmsg_from_mainwindow(QString msg)
{
    this->ui->edit_recvmsg->insertPlainText("[server]:\n" + msg + "\n");
}

void QT_SocketClient::slot_receive_disconncet_from_mainwindow(QString msg)
{
    this->ui->edit_recvmsg->insertPlainText(msg);
    this->ui->btn_connect->setEnabled(true);
    this->ui->btn_disconnect->setEnabled(false);
}

void QT_SocketClient::slot_receive_error_from_mainwindow(QString msg)
{
    this->ui->edit_recvmsg->insertPlainText(msg);
    this->ui->btn_connect->setEnabled(true);
    this->ui->btn_disconnect->setEnabled(false);
}
