#ifndef QT_SOCKETSERVER_H
#define QT_SOCKETSERVER_H

#include <QWidget>

namespace Ui
{
    class QT_SocketServer;
}

class QT_SocketServer : public QWidget
{
    Q_OBJECT

public:
    explicit QT_SocketServer(QWidget *parent = nullptr);
    ~QT_SocketServer();

public slots:
    void slot_listeningbtn_clicked();
    void slot_stoplisteningbtn_clicked();
    void slot_sendbtn_clicked();

    //receive connect client msg
    void slot_receive_clientmsg_from_mainwindow(QString ip, quint16 port);

    //receive disconnect client msg
    void slot_receive_disconnect_clientms_from_mainwindow(QString ip, quint16 port);

    //receive errormsg from mainwindow
    void slot_receive_error_from_mainwindow(QString ip, quint16 port, QString error);

    //receive common msg and show it in text_edit
    void slot_receive_commonmsg_from_mainwindow(QString ip, quint16 port, QString msg);



signals:
    void signal_startlistening_toclient( QString ip, quint16 port);
    void signal_send_msg_to_client(QString client_index, char typenumber, QString msg);
    void signal_stop_listening_to_client();
private:
    Ui::QT_SocketServer *ui;
};

#endif // QT_SOCKETSERVER_H
