#ifndef QT_SOCKETCLIENT_H
#define QT_SOCKETCLIENT_H

#include <QWidget>

namespace Ui
{
    class QT_SocketClient;
}

class QT_SocketClient : public QWidget
{
    Q_OBJECT

public:
    explicit QT_SocketClient(QWidget *parent = nullptr);
    ~QT_SocketClient();


public slots:
    //connect btn clicked
    void slot_connectbtn_clicked();

    //disconnect btn clicked
    void slot_disconncet_clicked();

    //send btn clicked
    void slot_sendbtn_clicked();


    // receive connect msg form mainwindow
    void slot_receive_connect_from_mainwindow();

    //receive common msg from mainwindow
    void slot_receive_commonmsg_from_mainwindow(QString msg);

    //receive disconnect msg from mainwindow
    void slot_receive_disconncet_from_mainwindow(QString msg);

    //receive error
    void slot_receive_error_from_mainwindow(QString msg);

signals:
    void signal_connect_to_server(QString ip, quint16 port);

    void signal_disconnect_to_server();


    void signal_send_msg_to_server(char typenumber, QString msg);

private:
    Ui::QT_SocketClient *ui;
};

#endif // QT_SOCKETCLIENT_H
