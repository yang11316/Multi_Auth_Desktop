#include "qt_initializekgc.h"
#include "ui_qt_initializekgc.h"

QT_InitializeKGC::QT_InitializeKGC(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QT_InitializeKGC)
{
    ui->setupUi(this);
    this->ui->btn_restart->setEnabled(false);
    connect(this->ui->btn_start, &QPushButton::clicked, this, &QT_InitializeKGC::slot_startbtn_clicked);
    connect(this->ui->btn_restart, &QPushButton::clicked, this, &QT_InitializeKGC::slot_resetbtn_clicked);
}

QT_InitializeKGC::~QT_InitializeKGC()
{
    delete ui;
}

void QT_InitializeKGC::slot_startbtn_clicked()
{
    this->ui->btn_start->setEnabled(false);
    this->ui->btn_restart->setEnabled(true);
    emit signal_initialbtn_to_mainwindow();
}

void QT_InitializeKGC::slot_resetbtn_clicked()
{
    this->ui->btn_start->setEnabled(true);
    this->ui->btn_restart->setEnabled(false);
    this->ui->edit_text->clear();
    emit signal_resetkgc_to_mainwindow();
}

void QT_InitializeKGC::slot_receive_acc_kgc_initial_from_mainwindow()
{
    this->ui->btn_start->setEnabled(false);
    this->ui->btn_restart->setEnabled(true);
}



void QT_InitializeKGC::slot_receive_message_from_mainwindow(QString str)
{

    this->ui->edit_text->append(str);
}
