#ifndef QT_CONNECTCLIENT_H
#define QT_CONNECTCLIENT_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>
#include <QDataStream>

#include "loader.h"

namespace Ui
{
    class QT_ConnectClient;
}


struct  QPayload
{
    QString pid;
    QString msg;
    QString sig1, sig2;
    QString pk1, pk2;
    QString time_stamp;
    QString wit_new;
    QString file_name;
    QString file_hash;

    // 重写QDataStream& operator<<操作符，做数据序列化操作
    friend QDataStream &operator<<(QDataStream &stream, const QPayload &tmp_parameter)
    {
        // 将数据输入流对象中
        stream << tmp_parameter.pid;
        stream << tmp_parameter.msg;
        stream << tmp_parameter.sig1;
        stream << tmp_parameter.sig2;
        stream << tmp_parameter.pk1;
        stream << tmp_parameter.pk2;
        stream << tmp_parameter.time_stamp;
        stream << tmp_parameter.wit_new;
        stream << tmp_parameter.file_name;
        stream << tmp_parameter.file_hash;

        return stream;
    }

    // 重写QDataStream& operator>>操作符，做数据反序列化操作
    friend QDataStream &operator>>(QDataStream &stream, QPayload &tmp_parameter)
    {
        // 从流对象中输出数据到学生结构体引用中
        stream >> tmp_parameter.pid;
        stream >> tmp_parameter.msg;
        stream >> tmp_parameter.sig1;
        stream >> tmp_parameter.sig2;
        stream >> tmp_parameter.pk1;
        stream >> tmp_parameter.pk2;
        stream >> tmp_parameter.time_stamp;
        stream >> tmp_parameter.wit_new;
        stream >> tmp_parameter.file_name;
        stream >> tmp_parameter.file_hash;
        return stream;
    }



};




struct File_Hash_List
{
    QList<QString> file_name;
    QList<QString> file_hash;

    // 重写QDataStream& operator<<操作符，做数据序列化操作
    friend QDataStream &operator<<(QDataStream &stream, const File_Hash_List &tmp_parameter)
    {
        // 将数据输入流对象中
        stream << tmp_parameter.file_name;
        stream << tmp_parameter.file_hash;
        return stream;
    }

    // 重写QDataStream& operator>>操作符，做数据反序列化操作
    friend QDataStream &operator>>(QDataStream &stream, File_Hash_List &tmp_parameter)
    {
        // 从流对象中输出数据到学生结构体引用中
        stream >> tmp_parameter.file_name;
        stream >> tmp_parameter.file_hash;
        return stream;
    }
};



class QT_ConnectClient : public QWidget
{
    Q_OBJECT

public:
    explicit QT_ConnectClient(QWidget *parent = nullptr);
    ~QT_ConnectClient();

    void read_file_json();

    void send_file_struct();

    void calculate_file_payload();

    void receive_file_struct();
    void receive_qpayload_struct();


public slots:
    void slot_connect_btn_clicked();
    void slot_listen_btn_clicked();
    void slot_stoplisten_btn_clicked();
    void slot_disconnect_btn_clicked();
    void slot_send_btn_clicked();



    void slot_newconnection_from_client();
    void slot_readdata_from_client();
    void slot_dissconect_from_client();

    void slot_deal_receive_msg(char typenumber);

    void slot_receive_qpayload_from_mainwindow(QPayload qpayload);

    void slot_receive_msg_from_mainwindow(QString msg);

signals:
    void signal_deal_receive_msg(char typenumber);

    void signal_calculate_file_payload(QString local_file_path, QString msg, QString target_file_name,
                                       QString target_file_hash);
    void signal_deal_receive_qpayload_to_mainwindow(QPayload qpayload);
private:
    Ui::QT_ConnectClient *ui;

    QMap<QString, QString> map_filepath_pid;
    QMap<QString, QString> map_filehash_filepath;

    QHostAddress *listening_ip;

    QTcpServer *client_server;
    QTcpSocket *client_socket;
    QTcpSocket *client_send_socket;

    QByteArray connect_Serialize_receive_dataArray;
    QByteArray connect_Serialize_send_dataArray;


    QString save_file_path = "calculated_file.json";
};

#endif // QT_CONNECTCLIENT_H
