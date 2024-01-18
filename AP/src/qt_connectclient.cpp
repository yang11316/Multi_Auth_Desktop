#include "qt_connectclient.h"
#include "ui_qt_connectclient.h"

QT_ConnectClient::QT_ConnectClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QT_ConnectClient)
{
    ui->setupUi(this);
    this->ui->stop_btn->setEnabled(false);
    this->ui->disconnect_btn->setEnabled(false);

    this->client_socket = new QTcpSocket(this);
    this->client_send_socket = new QTcpSocket(this);
    this->client_server = new QTcpServer(this);

    connect(this->ui->listen_btn, &QPushButton::clicked, this, &QT_ConnectClient::slot_listen_btn_clicked);
    connect(this->ui->stop_btn, &QPushButton::clicked, this, &QT_ConnectClient::slot_stoplisten_btn_clicked);
    connect(this->client_server, &QTcpServer::newConnection, this, &QT_ConnectClient::slot_newconnection_from_client);
    connect(this, &QT_ConnectClient::signal_deal_receive_msg, this, &QT_ConnectClient::slot_deal_receive_msg);
    connect(this->ui->connect_btn, &QPushButton::clicked, this, &QT_ConnectClient::slot_connect_btn_clicked);
    connect(this->ui->disconnect_btn, &QPushButton::clicked, this, &QT_ConnectClient::slot_disconnect_btn_clicked);
    connect(this->ui->send_btn, &QPushButton::clicked, this, &QT_ConnectClient::slot_send_btn_clicked);


    connect(this->client_send_socket, &QTcpSocket::disconnected, this, [ = ]()
    {
        this->ui->receive_edit->append("[AP]disconncet\n");
        ui->disconnect_btn->setEnabled(false);
        ui->connect_btn->setEnabled(true);
        ui->listen_btn->setEnabled(true);
        ui->stop_btn->setEnabled(true);
    });
    connect(this->client_send_socket, &QTcpSocket::connected, this, [ = ]()
    {
        // 将成功连接显示到通信记录编辑栏中
        ui->receive_edit->append("[AP]connect\n");
        // 将断开连接置成可用状态
        ui->disconnect_btn->setEnabled(true);
        ui->connect_btn->setEnabled(false);
        ui->listen_btn->setEnabled(false);
        ui->stop_btn->setEnabled(false);

    });

    connect(this->client_send_socket, &QTcpSocket::readyRead, this, &QT_ConnectClient::slot_readdata_from_client);


    read_file_json();
}

QT_ConnectClient::~QT_ConnectClient()
{
    delete ui;
}

void QT_ConnectClient::read_file_json()
{

    this->ui->local_listWidget->clear();
    std::ifstream ifs;
    ifs.open(this->save_file_path.toStdString());
    if (ifs.good())
    {
        std::vector<file_struct> vector_file_json;
        ifs.close();
        vector_file_json = loader::read_file_json(this->save_file_path.toStdString());
        for (int i = 0; i < vector_file_json.size(); ++i)
        {
            this->map_filepath_pid.insert(QString::fromStdString(vector_file_json[i].file_path),
                                          QString::fromStdString(vector_file_json[i].file_pid));

            this->map_filehash_filepath.insert(QString::fromStdString(vector_file_json[i].file_md5),
                                               QString::fromStdString(vector_file_json[i].file_path));
            QString file_name = QString::fromStdString( vector_file_json[i].file_name);
            QString file_path = QString::fromStdString( vector_file_json[i].file_path);
            QListWidgetItem *pItem = new QListWidgetItem;
            pItem->setData(
                Qt::UserRole,
                file_path);
            pItem->setSizeHint(QSize(0, 30));
            pItem->setText(file_name);
            this->ui->local_listWidget->addItem(pItem);
        }
    }
}

void QT_ConnectClient::send_file_struct()
{

    struct File_Hash_List file_hash_list;

    std::ifstream ifs;
    ifs.open(this->save_file_path.toStdString());
    if (ifs.good())
    {
        std::vector<file_struct> vector_file_json;
        ifs.close();
        vector_file_json = loader::read_file_json(this->save_file_path.toStdString());
        for (int i = 0; i < vector_file_json.size(); ++i)
        {
            file_hash_list.file_name.append(QString::fromStdString( vector_file_json[i].file_name));
            file_hash_list.file_hash.append(QString::fromStdString(vector_file_json[i].file_md5));
        }
    }
    // 创建一个数据流对象，并设置数据容器和打开方式
    QDataStream stream(&this->connect_Serialize_send_dataArray, QIODevice::WriteOnly);
    // 通过'<<'运算符将数据写入
    stream << file_hash_list;
    char typenumber = '4';
    if (this->client_socket->state() == QAbstractSocket::ConnectedState)
    {
        int msg_size = this->connect_Serialize_send_dataArray.size();
        this->client_socket->write((char *)&msg_size, 4);
        this->client_socket->write(&typenumber, 1);
        this->client_socket->write(this->connect_Serialize_send_dataArray, this->connect_Serialize_send_dataArray.size());
    }
    else if (this->client_send_socket->state() == QAbstractSocket::ConnectedState)
    {
        int msg_size = this->connect_Serialize_send_dataArray.size();
        this->client_send_socket->write((char *)&msg_size, 4);
        this->client_send_socket->write(&typenumber, 1);
        this->client_send_socket->write(this->connect_Serialize_send_dataArray, this->connect_Serialize_send_dataArray.size());
    }
    else
    {
        this->ui->receive_edit->append("socket disconnected, can't send msg\n");
    }
    this->connect_Serialize_send_dataArray.clear();
    this->connect_Serialize_send_dataArray.resize(0);

}

void QT_ConnectClient::calculate_file_payload()
{
    QString msg = this->ui->send_edit->toPlainText();
    QListWidgetItem *local_item = ui->local_listWidget->currentItem();
    QListWidgetItem *target_item = ui->target_lsitWidget->currentItem();
    if (local_item == nullptr)
    {
        return;
    }
    if (target_item == nullptr)
    {
        return;
    }
    QString file_path = local_item->data(Qt::UserRole).toString();
    QString target_file_name = target_item->text();
    QString target_file_hash = target_item->data(Qt::UserRole).toString();
    emit signal_calculate_file_payload(file_path, msg, target_file_name, target_file_hash);


}

void QT_ConnectClient::receive_file_struct()
{
    // 创建一个数据流对象，并设置数据容器和打开方式
    QDataStream stream(&this->connect_Serialize_receive_dataArray, QIODevice::ReadOnly);
    struct File_Hash_List file_hash_list;
    stream >> file_hash_list;

    for (int i = 0; i < file_hash_list.file_name.size(); i++)
    {
        QListWidgetItem *pItem = new QListWidgetItem;
        pItem->setData(
            Qt::UserRole,
            file_hash_list.file_hash[i]);
        pItem->setSizeHint(QSize(0, 30));
        pItem->setText(file_hash_list.file_name[i]);
        this->ui->target_lsitWidget->addItem(pItem);
    }



    this->connect_Serialize_receive_dataArray.clear();
    this->connect_Serialize_receive_dataArray.resize(0);
}

void QT_ConnectClient::receive_qpayload_struct()
{

    // 创建一个数据流对象，并设置数据容器和打开方式
    QDataStream stream(&this->connect_Serialize_receive_dataArray, QIODevice::ReadOnly);
    struct QPayload qpayload;
    stream >> qpayload;
    emit signal_deal_receive_qpayload_to_mainwindow(qpayload);

}

void QT_ConnectClient::slot_connect_btn_clicked()
{
    QString target_ip = this->ui->connect_ip_edit->text();
    quint16 target_port = this->ui->connect_port_edit->value();
    this->client_send_socket->connectToHost(QHostAddress(target_ip), target_port);
}

void QT_ConnectClient::slot_listen_btn_clicked()
{
    QString listen_ip = this->ui->listen_ip_edit->text();
    quint16 listen_port = this->ui->listen_port_edit->value();
    this->listening_ip = new QHostAddress();
    if (!listening_ip->setAddress(listen_ip))
    {
        //用这个函数判断IP地址是否可以被正确解析
        //不能被正确解析就发出错误信号
        QMessageBox::warning(this, "warning", "ip error");
        return;
    }
    this->ui->listen_btn->setEnabled(false);
    this->ui->connect_btn->setEnabled(false);
    this->ui->stop_btn->setEnabled(true);
    this->client_server->listen(*(this->listening_ip), listen_port);
}

void QT_ConnectClient::slot_stoplisten_btn_clicked()
{
    this->client_socket->close();
    this->client_server->close();


    this->ui->listen_btn->setEnabled(true);
    this->ui->connect_btn->setEnabled(true);
    this->ui->disconnect_btn->setEnabled(true);
    this->ui->stop_btn->setEnabled(false);

}

void QT_ConnectClient::slot_disconnect_btn_clicked()
{
    this->client_send_socket->abort();
    this->ui->receive_edit->append("disconncet AP sucessfully\n");
}

void QT_ConnectClient::slot_send_btn_clicked()
{

    if (this->ui->loacl_file_checkbox->isChecked())
    {
        send_file_struct();
        return;
    }
    else
    {
        if (!this->ui->send_edit->document()->isEmpty())
        {
            calculate_file_payload();
        }
    }

}

void QT_ConnectClient::slot_newconnection_from_client()
{
    //获取客户端连接
    this->client_socket = this->client_server->nextPendingConnection();
    if (this->client_socket == nullptr)
    {
        return;
    }
    QString client_ip = this->client_socket->peerAddress().toString();
    quint16 client_port = this->client_socket->peerPort();
    //    把新进来的客户端存入列表
    this->ui->receive_edit->append("已接收连接：" + client_ip + ":" + QString::number( client_port) + "\n");
    this->ui->connect_btn->setEnabled(false);
    this->ui->disconnect_btn->setEnabled(false);
    //连接QTcpSocket的信号槽，以读取新数据
    QObject::connect(this->client_socket, &QTcpSocket::readyRead, this, &QT_ConnectClient::slot_readdata_from_client);
    QObject::connect(this->client_socket, &QTcpSocket::disconnected, this, &QT_ConnectClient::slot_dissconect_from_client);

}

void QT_ConnectClient::slot_readdata_from_client()
{

    int count = 0;
    int total = 0;
    char typenumber;

    QTcpSocket *tmp_socket;
    if (this->client_socket->state() == QAbstractSocket::ConnectedState)
    {
        tmp_socket = client_socket;
    }
    else if (this->client_send_socket->state() == QAbstractSocket::ConnectedState)
    {
        tmp_socket = client_send_socket;
    }
    else
    {
        return;
    }

    while (1)
    {
        if (count == 0)
        {
            tmp_socket->read((char *)&total, 4);
            tmp_socket->read(&typenumber, 1);
        }
        QByteArray all = tmp_socket->readAll();
        count += all.size();
        this->connect_Serialize_receive_dataArray += all;
        if (count == total)
        {

            emit signal_deal_receive_msg(typenumber);
            return;
        }
        else if (count  > total)
        {
            QString client_ip = this->client_socket->peerAddress().toString();
            quint16 client_port = this->client_socket->peerPort();
            this->ui->receive_edit->append(client_ip + QString::number( client_port) + "receive error: count > total\n");
            tmp_socket->close();
            tmp_socket->deleteLater();
            return;
        }

    }
}

void QT_ConnectClient::slot_dissconect_from_client()
{


    this->ui->receive_edit->append("Ap disconnect\n");
    this->client_socket->deleteLater();


}

void QT_ConnectClient::slot_deal_receive_msg(char typenumber)
{
    switch (typenumber)
    {
        case '4':
            receive_file_struct();
            break;
        case '6':
            receive_qpayload_struct();
            break;
        default:
            break;
    }
}

void QT_ConnectClient::slot_receive_qpayload_from_mainwindow(QPayload qpayload)
{
    // 创建一个数据流对象，并设置数据容器和打开方式
    QDataStream stream(&this->connect_Serialize_send_dataArray, QIODevice::WriteOnly);
    // 通过'<<'运算符将数据写入
    stream << qpayload;
    char typenumber = '6';
    if (this->client_socket->state() == QAbstractSocket::ConnectedState)
    {
        int msg_size = this->connect_Serialize_send_dataArray.size();
        this->client_socket->write((char *)&msg_size, 4);
        this->client_socket->write(&typenumber, 1);
        this->client_socket->write(this->connect_Serialize_send_dataArray, this->connect_Serialize_send_dataArray.size());
    }
    else if (this->client_send_socket->state() == QAbstractSocket::ConnectedState)
    {
        int msg_size = this->connect_Serialize_send_dataArray.size();
        this->client_send_socket->write((char *)&msg_size, 4);
        this->client_send_socket->write(&typenumber, 1);
        this->client_send_socket->write(this->connect_Serialize_send_dataArray, this->connect_Serialize_send_dataArray.size());
    }
    else
    {
        this->ui->receive_edit->append("socket disconnected, can't send msg\n");
    }
    this->ui->receive_edit->append("已发送签名信息\n");
    this->connect_Serialize_send_dataArray.clear();
    this->connect_Serialize_send_dataArray.resize(0);



}

void QT_ConnectClient::slot_receive_msg_from_mainwindow(QString msg)
{
    this->ui->receive_edit->append(msg);
}


