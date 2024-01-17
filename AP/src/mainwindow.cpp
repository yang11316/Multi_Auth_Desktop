#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(this->width(), this->height());

    ui->stackedWidget->addWidget(&qt_socketclient);
    ui->stackedWidget->addWidget(&qt_filemanage);
    ui->stackedWidget->addWidget(&qt_connectclient);

    btnGroup.addButton(ui->btn_connect, 0);
    btnGroup.addButton(ui->btn_filepage, 1);
    btnGroup.addButton(ui->btn_connectclient, 2);

    connect(
        &btnGroup,
        static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
        ui->stackedWidget, &QStackedWidget::setCurrentIndex);

    // 设置默认选中的页面
    btnGroup.button(0)->setChecked(true);
    ui->stackedWidget->setCurrentIndex(0);

    this->save_file_path = QApplication::applicationDirPath() + "/file";
    QDir dir(QDir::currentPath());
    if (!dir.exists("file"))
    {
        dir.mkdir("file");
    }



    this->socket = new QTcpSocket(this);
    connect(this->socket, &QTcpSocket::connected, this, [ = ]()
    {
        emit signal_server_connect_to_socketpage();
    });
    connect(this->socket, &QTcpSocket::readyRead, this, &MainWindow::slot_receive_msg_from_server);
    connect(this->socket, &QTcpSocket::disconnected, this, [ = ]()
    {
        emit signal_server_disconnect_to_socketpage("[server]disconncet\n");
    });

    connect(&this->qt_socketclient, &QT_SocketClient::signal_connect_to_server, this, &MainWindow::slot_connect_server_from_socketpage);
    connect(&this->qt_socketclient, &QT_SocketClient::signal_disconnect_to_server, this, [ = ]()
    {
        this->socket->abort();
        emit signal_server_disconnect_to_socketpage("[client]disconncet sucessfully\n");
    });
    connect(&this->qt_socketclient, &QT_SocketClient::signal_send_msg_to_server, this, &MainWindow::slot_deal_sendmsg_from_socketpage);


    connect(this, &MainWindow::signal_send_error_to_socketpage, &this->qt_socketclient, &QT_SocketClient::slot_receive_error_from_mainwindow);
    connect(this, &MainWindow::signal_server_disconnect_to_socketpage, &this->qt_socketclient, &QT_SocketClient::slot_receive_disconncet_from_mainwindow);
    connect(this, &MainWindow::signal_server_connect_to_socketpage, &this->qt_socketclient, &QT_SocketClient::slot_receive_connect_from_mainwindow);
    connect(this, &MainWindow::signal_send_commonmsg_to_socketpage, &this->qt_socketclient, &QT_SocketClient::slot_receive_commonmsg_from_mainwindow);

    connect(this, &MainWindow::signal_deal_receivedmsg_to_mainwindow, this, &MainWindow::slot_deal_receivemsg_from_server);
    connect(&this->qt_filemanage, &QT_FileManage::signal_cal_file_fullkey_to_mainwindow, this, &MainWindow::slot_calculate_file_fullkey_from_filemanage);
    connect(this, &MainWindow::signal_send_msg_to_filemanage, &this->qt_filemanage, &QT_FileManage::slot_receive_msg_from_mainwindow);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::send_common_msg_to_server(char typenumber, QString msg)
{
    int msg_size = msg.size();
    this->socket->write((char *)&msg_size, 4);
    this->socket->write(&typenumber, 1);
    this->socket->write(msg.toUtf8().data(), msg.length());
}



void MainWindow::receive_common_msg_from_server()
{

}

void MainWindow::receive_acc_publickey_from_server()
{
    this->acc_public_key =  QString( this->Serialize_receive_dataArray);
    emit signal_send_commonmsg_to_socketpage("acc public key:" + this->acc_public_key );
    this->Serialize_receive_dataArray.clear();
    this->Serialize_receive_dataArray.resize(0);
}


void MainWindow::receive_kgc_parameter_from_server()
{


    // 创建一个数据流对象，并设置数据容器和打开方式
    QDataStream stream(&this->Serialize_receive_dataArray, QIODevice::ReadOnly);
    stream >> this->kgc_parameter;
    QString msg = this->kgc_parameter.Ppub_hex + QString::number(kgc_parameter.k);
    emit signal_send_commonmsg_to_socketpage("kgc parameter:" + msg);
    this->Serialize_receive_dataArray.clear();
    this->Serialize_receive_dataArray.resize(0);

}

void MainWindow::receive_file_info_from_server()
{
    this->file_flag = false;
    QString msg =  QString( this->Serialize_receive_dataArray);
    this->send_file_name = msg.section("##", 0, 0);    // 分割字符串
    this->send_file_size = msg.section("##", 1, 1).toInt();

    QString file_pid = msg.section("##", 2, 2);
    QString file_hash = msg.section("##", 3, 3);
    QString partial_key1 = msg.section("##", 4, 4);
    QString partial_key2 = msg.section("##", 5, 5);
    QString file_acc_wit =  msg.section("##", 6, 6);



    QString file_path = this->save_file_path + "/" + send_file_name;

    this->receive_file_size = 0;
    signal_send_commonmsg_to_socketpage("收到文件头信息\nfilename:" + this->send_file_name +
                                        "\nfilesize:" + QString::number(this->send_file_size) + "\n"
                                        + "部分私钥:" + partial_key1 + "\n" + partial_key2);
    // 打开文件
    file.setFileName("./file/" + this->send_file_name);
    bool isOK = file.open(QIODevice::WriteOnly);
    if (false == isOK)
    {
        signal_send_commonmsg_to_socketpage(this->send_file_name + "文件写入失败\n");
        return ;
    }
    std::string filehash = file_hash.toStdString();
    std::string filepid = file_pid.toStdString();
    if (loader::write_json_single("calculated_file.json", this->send_file_name.toStdString(), file_path.toStdString(),
                                  filehash, filepid, partial_key1.toStdString(), partial_key2.toStdString(), file_acc_wit.toStdString()))
    {
        qDebug() << "文件信息写入json文件";
    }


    this->Serialize_receive_dataArray.clear();
    this->Serialize_receive_dataArray.resize(0);
}

void MainWindow::receive_file_body_from_server()
{
    quint64 len = file.write(this->Serialize_receive_dataArray);
    if (len > 0)
    {
        this->receive_file_size += len;
    }
    if (this->receive_file_size == this->send_file_size)
    {
        file.close();
        signal_send_commonmsg_to_socketpage(this->send_file_name + "文件传输完成\n");
        //        QMessageBox::information(this, "完成", "文件接收完成");
        this->file_flag = true;
        this->socket->readAll();
        this->Serialize_receive_dataArray.clear();
        this->Serialize_receive_dataArray.resize(0);
    }
    this->Serialize_receive_dataArray.clear();
    this->Serialize_receive_dataArray.resize(0);
    slot_receive_msg_from_server();
    this->qt_filemanage.slot_update_filewidget();

}

void MainWindow::slot_connect_server_from_socketpage(QString ip, quint16 port)
{
    this->server_ip = ip;
    this->server_port = port;
    this->socket->connectToHost(QHostAddress(server_ip), server_port);
}

void MainWindow::slot_receive_msg_from_server()
{
    quint64 count = 0;
    quint64 total = 0;
    char typenumber;
    while (1)
    {
        if (count == 0)
        {
            this->socket->read((char *)&total, 4);
            this->socket->read(&typenumber, 1);

        }
        QByteArray tmp;
        if (total > 65536)
        {
            tmp = this->socket->readAll();
        }
        else
        {
            tmp = this->socket->read(total);
        }
        count += tmp.size();
        Serialize_receive_dataArray += tmp;
        if (count == total)
        {

            emit signal_deal_receivedmsg_to_mainwindow(typenumber);
            return;
        }
        else if (count  > total)
        {
            emit signal_send_error_to_socketpage("receive error: count > total\n");

            //            this->m_tcp->close();
            //            this->m_tcp->deleteLater();
            return;
        }

    }
}

void MainWindow::slot_deal_receivemsg_from_server(char typenumber)
{
    switch (typenumber)
    {
        case '0':
            receive_common_msg_from_server();
            break;
        case '1':
            receive_kgc_parameter_from_server();
            break;
        case '3':
            receive_file_info_from_server();
            break;
        case '5':
            receive_file_body_from_server();
            break;
        case '7':
            receive_acc_publickey_from_server();
            break;

        default:
            break;
    }
}

void MainWindow::slot_deal_sendmsg_from_socketpage(char typenumber, QString msg)
{
    switch (typenumber)
    {
        case '0':
            send_common_msg_to_server(typenumber, msg);
            break;
        default:
            break;
    }
}

void MainWindow::slot_calculate_file_fullkey_from_filemanage(QString filepath)
{

    std::vector<std::string> partial_key = loader::get_file_partial_key(this->cal_file_path.toStdString(), filepath.toStdString()) ;
    std::string pid = loader::get_file_pid(this->cal_file_path.toStdString(), filepath.toStdString());
    int k = kgc_parameter.k;
    std::string Ppub_hex = kgc_parameter.Ppub_hex.toStdString();
    Process *process = new Process(pid, k, Ppub_hex);
    process->GenerateFullKey(partial_key);
    this->process_map.insert(filepath, process);
    this->qt_filemanage.fullkey_file.append(filepath);
    this->qt_filemanage.slot_update_filewidget();

    emit signal_send_msg_to_filemanage(filepath + "完整密钥计算完成\n");
}

