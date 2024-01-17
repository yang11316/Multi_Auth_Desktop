#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QMainWindow>
#include <QPainter>
#include <QPropertyAnimation>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDataStream>
#include <QMessageBox>
#include <QFile>
#include <QApplication>
#include <QDir>

#include "qt_socketclient.h"
#include "qt_filemanage.h"
#include "Data_struct.h"
#include "loader.h"
#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>

#include <iostream>
#include <string>

#include "KGC_pariFree.h"
#include "Process_pairFree.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //处理要发送的数据
    void send_common_msg_to_server(char typenumber, QString msg);


    // 处理读取到的数据
    void receive_acc_publickey_from_server();
    void receive_common_msg_from_server();
    void receive_kgc_parameter_from_server();
    void receive_file_info_from_server();
    void receive_file_body_from_server();

    //

public slots:
    void slot_connect_server_from_socketpage(QString ip, quint16 port);

    void slot_receive_msg_from_server();

    void slot_deal_receivemsg_from_server(char typenumber);

    void slot_deal_sendmsg_from_socketpage(char typenumber, QString msg);

    void slot_calculate_file_fullkey_from_filemanage(QString filepath);


signals:
    void signal_send_commonmsg_to_socketpage(QString msg);

    void signal_server_disconnect_to_socketpage(QString msg);

    void signal_deal_receivedmsg_to_mainwindow(char typenumber);

    void signal_send_error_to_socketpage(QString error);

    void signal_server_connect_to_socketpage();

private:
    Ui::MainWindow *ui;
    QButtonGroup btnGroup;

    QT_SocketClient qt_socketclient;
    QT_FileManage qt_filemanage;

    QTcpSocket *socket;
    quint16 server_port ;
    QString server_ip;

    struct Kgc_Parameter kgc_parameter;
    QMap<QString, Process *> process_map;


    QString acc_public_key;

    //存储序列化数据
    QByteArray Serialize_receive_dataArray;
    QByteArray Serialize_send_dataArray;

    QFile file;
    bool file_flag = true;
    QString send_file_name;
    quint64 send_file_size;
    quint64 receive_file_size;

    QString save_file_path;

    QString cal_file_path = "calculated_file.json";



};
#endif // MAINWINDOW_H
