#ifndef QT_MAINWINDOW_H
#define QT_MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QMainWindow>
#include <QPainter>
#include <QPropertyAnimation>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDataStream>
#include <QTimer>
#include <QEventLoop>

#include <iostream>
#include <string>

#include "KGC_pariFree.h"
#include "Process_pairFree.h"
#include "accumulator.h"


#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>

#include "qt_initializekgc.h"
#include "qt_filechoose.h"
#include "qt_socketserver.h"
#include "qt_fileissuance.h"
#include "Data_struct.h"

namespace Ui
{
    class QT_MainWindow;
}





//==================================================================
//类 名：  QT_MainWindow
//功    能： 控制三个页面切换,维护kgc类，累加器类，socket通信
//日    期： 2024/1/12
//版    本： 1.0
//==================================================================
class QT_MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QT_MainWindow(QWidget *parent = nullptr);
    ~QT_MainWindow();
    void check_acc_kgc_json();

    void sleep(int msec);

    //消息处理相关函数

    // send kgc parameter
    void send_kgc_parameter_to_client(QString client_index, char typenumber);
    // send comment message
    void send_comment_msg_to_client(QString cliennt_index, char typenumber, QString msg);

    void send_acc_publickey_to_client(QString client_index, char typenumber);

    void send_file_info_to_client(QString client_index,  char typenumber, QString filepath);

    void send_file_body_to_client();

    void receive_common_msg_from_client(QString client_ip, quint16 client_port);







public slots:
    //初始化kgc、累加器
    void slot_initialzeKGC_from_initialpage();
    //kgc、累加器重置清空
    void slot_resetKGC_from_initialpage();

    //start listening
    void slot_startlistenning_from_socketpage(QString ip, quint16 port);
    //新client到来进行连接
    void slot_newconnection_from_client();
    //    client断开连接处理
    void slot_dissconect_from_client();
    //读取client传来的数据
    void slot_readdata_from_client();

    //对消息进行处理
    void slot_deal_receive_data_from_client(QString ip, quint16 port, char typenumber);


    //从socketpage读取到的信息发送至client
    void slot_send_msg_to_client_from_socketpage(QString client_index, char typenumber, QString msg);

    //停止监听，关闭所有已存在连接
    void slot_stop_listening_from_socketpage();

    //接收fileissuance发出的file，并使用socket发送给client
    void slot_send_file_to_client(QString client_id, QList<QString>file_lsit);

    //计算filechoose发来的文件pid，累加器的值
    void slot_calculate_file_pid_fromfilechoosepage(QStringList filepathlist);


signals:
    //发送消息给initialpage
    void signal_message_to_initialpage(QString str);

    //向socketserver发送已连接的client消息
    void signal_clientmsg_to_socketpage(QString ip, quint16 port);

    //向socketserver发送断开的client信息
    void signal_disconnect_clientmsg_to_socketpage(QString ip, quint16 port);

    //向socketpage发送error信息
    void signal_send_errormsg_to_socketpage(QString ip, quint16 port, QString error);

    //根据typenumber处理send信息
    void signal_deal_sendmsg_to_mainwindow(QString ip, quint16 port, char typenumber, QString msg);

    //根据typenumber处理receive信息
    void signal_deal_receivedmsg_to_mainwindow(QString ip, quint16 port, char typenumber);

    //向socketpage发送common信息
    void signal_send_commonmsg_to_sockpage(QString ip, quint16 port, QString msg);

    //向fileissuance发送client变更后的信息
    void signal_change_client_to_fileissuancepage(QList<QString> str_lsit);


    //向initialzekgc页面发送初始化kgc成功消息
    void signal_check_acc_kgc_file_sucessfull();



private:
    Ui::QT_MainWindow *ui;
    QButtonGroup btnGroup;
    QT_InitializeKGC qt_initialKGC;
    QT_FileChoose qt_filechoose;
    QT_SocketServer qt_socketserver;
    QT_Fileissuance qt_fileissuance;

    int k = 714;
    KGC *kgc_ptr;
    QString Ppubhex;
    Accumulator *acc_ptr;

    //通信相关类成员
    QTcpServer *server;
    QTcpSocket *socket;

    QHostAddress *listening_ip = nullptr;
    QMap<QString, QTcpSocket *> TcpSockets_List;
    QList<QString> client_list;


    int Client_num = 0;

    //存储序列化数据
    QByteArray Serialize_send_dataArray;
    QByteArray Serialize_receive_dataArray;

    QFile file;
    QString send_file_path;
    QString send_file_name;
    quint64 send_file_size;
    quint64 send_part_file_size;

    QString current_client_index;

    QTimer mytimer;
    QString savepath = "./calculated_file.json";
    QString client_json_path = "./client_file.json";
    QString acc_kgc_path = "./acc_kgc.json";
};

#endif // QT_MAINWINDOW_H
