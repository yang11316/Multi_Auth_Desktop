#include "qt_socketserver.h"
#include "ui_qt_socketserver.h"

QT_SocketServer::QT_SocketServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QT_SocketServer)
{
    ui->setupUi(this);
    connect(this->ui->listen_btn, &QPushButton::clicked, this, &QT_SocketServer::slot_listeningbtn_clicked);
    connect(this->ui->stop_btn, &QPushButton::clicked, this, &QT_SocketServer::slot_stoplisteningbtn_clicked);
    connect(this->ui->send_btn, &QPushButton::clicked, this, &QT_SocketServer::slot_sendbtn_clicked);

}

QT_SocketServer::~QT_SocketServer()
{
    delete ui;
}

void QT_SocketServer::slot_listeningbtn_clicked()
{
    this->ui->listen_btn->setEnabled(false);
    this->ui->stop_btn->setEnabled(true);
    QString listening_ip = this->ui->ip_edit->text();
    quint16 listening_port = this->ui->port_edit->value();

    emit signal_startlistening_toclient(listening_ip, listening_port);
}

void QT_SocketServer::slot_stoplisteningbtn_clicked()
{
    this->ui->client_listwidget->clear();
    this->ui->listen_btn->setEnabled(true);
    this->ui->stop_btn->setEnabled(false);
    emit signal_stop_listening_to_client();

}

void QT_SocketServer::slot_sendbtn_clicked()
{
    QListWidgetItem *item_now = ui->client_listwidget->currentItem();
    if (item_now == nullptr)
    {
        return;
    }
    else
    {
        QString client_index = item_now->data(Qt::UserRole).toString();
        if (this->ui->sendkgc_checkbox->isChecked())
        {
            emit signal_send_msg_to_client(client_index, '1', "");
        }
        if (this->ui->sendacc_checkbox->isChecked())
        {
            emit signal_send_msg_to_client(client_index, '7', "");
        }
        if (!this->ui->send_edit->document()->isEmpty())
        {
            QString msg = this->ui->send_edit->toPlainText();
            emit signal_send_msg_to_client(client_index, '0', msg);
        }
    }
}

void QT_SocketServer::slot_receive_clientmsg_from_mainwindow( QString ip, quint16 port)
{
    QListWidgetItem *pitem = new QListWidgetItem ();
    pitem->setText(ip + ":" + QString::number( port));
    pitem->setData(Qt::UserRole, ip + ":" + QString::number( port));
    this->ui->client_listwidget->addItem(pitem);
    this->ui->recv_edit->insertPlainText("[" + ip + ":" + QString::number(port) + "]:connect\n");
}

void QT_SocketServer::slot_receive_disconnect_clientms_from_mainwindow(QString ip, quint16 port)
{
    for (int i = 0; i < this->ui->client_listwidget->count(); i++)
    {
        QListWidgetItem *pitem  = ui->client_listwidget->item(i);
        QString tmp_data =  pitem->data(Qt::UserRole).toString();
        if ((ip + ":" + QString::number( port) ) == tmp_data)
        {
            QString tmp = "[" + pitem->text() + "]:disconnect\n";
            this->ui->recv_edit->insertPlainText(tmp);
            this->ui->client_listwidget->takeItem(i);
            break;
        }
    }
}

void QT_SocketServer::slot_receive_error_from_mainwindow(QString ip, quint16 port, QString error)
{
    this->ui->recv_edit->insertPlainText("[" + ip + ":" + QString::number( port) + "]error:\n" + error);
}

void QT_SocketServer::slot_receive_commonmsg_from_mainwindow(QString ip, quint16 port, QString msg)
{
    this->ui->recv_edit->insertPlainText("[" + ip + ":" + QString::number( port) + "]send message:\n" + msg);
}

