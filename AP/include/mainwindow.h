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
#include "qt_connectclient.h"


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

struct Kgc_Parameter
{
    QString Ppub_hex;
    int k;

    // 重写QDataStream& operator<<操作符，做数据序列化操作
    friend QDataStream &operator<<(QDataStream &stream, const Kgc_Parameter &tmp_parameter)
    {
        // 将数据输入流对象中
        stream << tmp_parameter.Ppub_hex;
        stream << tmp_parameter.k;
        return stream;
    }

    // 重写QDataStream& operator>>操作符，做数据反序列化操作
    friend QDataStream &operator>>(QDataStream &stream, Kgc_Parameter &tmp_parameter)
    {
        // 从流对象中输出数据到学生结构体引用中
        stream >> tmp_parameter.Ppub_hex;
        stream >> tmp_parameter.k;
        return stream;
    }

    // 重写QDebug operator<<函数，用作输出内容
    friend QDebug operator<<(QDebug debug, const Kgc_Parameter &tmp_parametert)
    {
        // 向QDebug对象输入数据
        debug << "Ppub_hex " << tmp_parametert.Ppub_hex;
        debug << "\nk " << tmp_parametert.k;
        debug << "\n";
        return debug;
    }
};




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

    void slot_calculate_file_payload_from_connectclient(QString local_file_path, QString msg, QString target_file_name,
            QString target_file_hash);

    void slot_receive_get_qpayload_from_connectclient(QPayload qpayload);

signals:
    void signal_send_commonmsg_to_socketpage(QString msg);

    void signal_server_disconnect_to_socketpage(QString msg);

    void signal_deal_receivedmsg_to_mainwindow(char typenumber);

    void signal_send_error_to_socketpage(QString error);

    void signal_server_connect_to_socketpage();

    void signal_send_msg_to_filemanage(QString msg);

    void signal_send_qpayload_to_connectclient(QPayload qpayload);

    void signal_send_msg_to_connectclient(QString msg);

private:
    Ui::MainWindow *ui;
    QButtonGroup btnGroup;

    QT_SocketClient qt_socketclient;
    QT_FileManage qt_filemanage;
    QT_ConnectClient qt_connectclient;

    QTcpSocket *socket;
    quint16 server_port ;
    QString server_ip;

    struct Kgc_Parameter kgc_parameter;
    QMap<QString, Process *> process_map;


    QString acc_public_key;
    QString acc_currnet;

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
