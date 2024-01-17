#include "qt_connectclient.h"
#include "ui_qt_connectclient.h"

QT_ConnectClient::QT_ConnectClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QT_ConnectClient)
{
    ui->setupUi(this);
}

QT_ConnectClient::~QT_ConnectClient()
{
    delete ui;
}
