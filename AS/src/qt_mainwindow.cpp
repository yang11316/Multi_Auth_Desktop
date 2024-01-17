#include "qt_mainwindow.h"
#include "ui_qt_mainwindow.h"

QT_MainWindow::QT_MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QT_MainWindow)
{
    ui->setupUi(this);


    ui->stackedWidget->addWidget(&this->qt_initialKGC);
    ui->stackedWidget->addWidget(&this->qt_filechoose);
    ui->stackedWidget->addWidget(&this->qt_socketserver);
    ui->stackedWidget->addWidget(&this->qt_fileissuance);


    btnGroup.addButton(ui->btn_home, 0);
    btnGroup.addButton(ui->btn_filechose, 1);
    btnGroup.addButton(ui->btn_socket, 2);
    btnGroup.addButton(ui->btn_fileissuance, 3);

    this->ui->btn_fileissuance->setEnabled(false);

    this->server = new QTcpServer();

    connect(
        &btnGroup,
        static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
        ui->stackedWidget, &QStackedWidget::setCurrentIndex);

    // 设置默认选中的页面
    btnGroup.button(0)->setChecked(true);
    ui->stackedWidget->setCurrentIndex(0);

    connect(&this->mytimer, &QTimer::timeout, this, &QT_MainWindow::send_file_body_to_client);


    connect(&this->qt_initialKGC, &QT_InitializeKGC::signal_initialbtn_to_mainwindow, this, &QT_MainWindow::slot_initialzeKGC_from_initialpage);
    connect(&this->qt_initialKGC, &QT_InitializeKGC::signal_resetkgc_to_mainwindow, this, &QT_MainWindow::slot_resetKGC_from_initialpage);
    connect(this, &QT_MainWindow::signal_check_acc_kgc_file_sucessfull, &this->qt_initialKGC, &QT_InitializeKGC::slot_receive_acc_kgc_initial_from_mainwindow);



    connect(this, &QT_MainWindow::signal_message_to_initialpage, &this->qt_initialKGC, &QT_InitializeKGC::slot_receive_message_from_mainwindow);


    connect(&this->qt_filechoose, SIGNAL(signal_calculate_file_to_mainwindow(QStringList)), this, SLOT(slot_calculate_file_pid_fromfilechoosepage(QStringList)));


    //    通绑定socket
    connect(&this->qt_socketserver, &QT_SocketServer::signal_startlistening_toclient, this, &QT_MainWindow::slot_startlistenning_from_socketpage);
    //    新的client连接
    connect(this->server, &QTcpServer::newConnection, this, &QT_MainWindow::slot_newconnection_from_client);
    //    发送新的client信息
    connect(this, &QT_MainWindow::signal_clientmsg_to_socketpage, &this->qt_socketserver, &QT_SocketServer::slot_receive_clientmsg_from_mainwindow);

    //    发送断开连接的client信息
    connect(this, &QT_MainWindow::signal_disconnect_clientmsg_to_socketpage, &this->qt_socketserver,
            &QT_SocketServer::slot_receive_disconnect_clientms_from_mainwindow);

    //发送错误信息给socketpage
    connect(this, &QT_MainWindow::signal_send_errormsg_to_socketpage, &this->qt_socketserver, &QT_SocketServer::slot_receive_error_from_mainwindow);
    //根据typenumber处理信息
    connect(this, &QT_MainWindow::signal_deal_receivedmsg_to_mainwindow, this, &QT_MainWindow::slot_deal_receive_data_from_client);

    connect(this, &QT_MainWindow::signal_send_commonmsg_to_sockpage, &this->qt_socketserver, &QT_SocketServer::slot_receive_commonmsg_from_mainwindow);

    //接收socketpage发送消息
    connect(&this->qt_socketserver, &QT_SocketServer::signal_send_msg_to_client, this, &QT_MainWindow::slot_send_msg_to_client_from_socketpage);

    //关闭所有连接，停止监听
    connect(&this->qt_socketserver, &QT_SocketServer::signal_stop_listening_to_client, this, &QT_MainWindow::slot_stop_listening_from_socketpage);

    //client状态变更信息通知到fileinssurance页面
    connect(this, SIGNAL( signal_change_client_to_fileissuancepage(QList<QString>)), &this->qt_fileissuance,
            SLOT(slot_change_client_from_mainwindow(QList<QString>)));
    connect(&this->qt_fileissuance, SIGNAL(signal_issuance_file_to_client(QString, QList<QString> )), this, SLOT(slot_send_file_to_client(QString,
            QList<QString> )));



    check_acc_kgc_json();
}


QT_MainWindow::~QT_MainWindow()
{
    delete ui;
}

void QT_MainWindow::check_acc_kgc_json()
{
    std::ifstream ifs;
    ifs.open(this->acc_kgc_path.toStdString());

    if (ifs.good())
    {
        ifs.close();
        acc_kgc_struct *this_struct ;
        this_struct = loader::read_acc_kgc_json(this->acc_kgc_path.toStdString());
        if (this_struct != nullptr)
        {
            emit signal_message_to_initialpage("【系统已初始化】\n");
            emit signal_message_to_initialpage("【认证模块】 初始化成功\n");

            emit signal_message_to_initialpage("【系统参数】 \n");
            emit signal_message_to_initialpage("public_key:" + QString::fromStdString( this_struct->acc_public_key) + "\n");
            emit signal_message_to_initialpage("serect key part1:" + QString::fromStdString(this_struct->acc_serect_part_one) + "\n");
            emit signal_message_to_initialpage("serect key part1:" + QString::fromStdString(this_struct->acc_serect_part_two) + "\n");
            emit signal_message_to_initialpage("acc init:" + QString::fromStdString(this_struct->acc_init) + "\n");
            this->acc_ptr = new Accumulator(_KEY_LEN);
            this->acc_ptr->public_key.set_str(this_struct->acc_public_key, 10);
            this->acc_ptr->secret_key.first.set_str(this_struct->acc_serect_part_one, 10);
            this->acc_ptr->secret_key.second.set_str(this_struct->acc_serect_part_two, 10);
            this->acc_ptr->acc_init.set_str(this_struct->acc_init, 10);
            this->acc_ptr->acc_cur = this->acc_ptr->acc_init;



            emit signal_message_to_initialpage("【KGC】 初始化成功 \n");
            emit signal_message_to_initialpage("【系统参数】 \n");
            this->kgc_ptr = new KGC(this_struct->kgc_s, this_struct->kgc_q, this_struct->kgc_k);

            this->Ppubhex = QString::fromStdString( point2hex(kgc_ptr->ec_group, kgc_ptr->P_pub));
            emit signal_message_to_initialpage(this->Ppubhex + "\n");

            this->ui->btn_fileissuance->setEnabled(true);

            emit signal_check_acc_kgc_file_sucessfull();
        }
    }
}

void QT_MainWindow::sleep(int msec)
{
    QEventLoop loop;//定义一个新的事件循环
    QTimer::singleShot(msec, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
    loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
}


void QT_MainWindow::send_kgc_parameter_to_client(QString client_index, char typenumber)
{
    QTcpSocket *tmp_client = this->TcpSockets_List.value(client_index);

    struct Kgc_Parameter tmp_parameter;
    tmp_parameter.Ppub_hex = this->Ppubhex;
    tmp_parameter.k = this->k;
    // 创建一个数据流对象，并设置数据容器和打开方式
    QDataStream stream(&Serialize_send_dataArray, QIODevice::WriteOnly);
    // 通过'<<'运算符将数据写入
    stream << tmp_parameter;

    int msg_size = Serialize_send_dataArray.size();
    tmp_client->write((char *)&msg_size, 4);
    tmp_client->write(&typenumber, 1);
    tmp_client->write(Serialize_send_dataArray, Serialize_send_dataArray.size());
    tmp_client = nullptr;
}

void QT_MainWindow::send_comment_msg_to_client(QString client_index, char typenumber, QString msg)
{
    QTcpSocket *tmp_client = this->TcpSockets_List.value(client_index);
    int msg_size = msg.size();
    tmp_client->write((char *)&msg_size, 4);
    tmp_client->write(&typenumber, 1);
    tmp_client->write(msg.toUtf8().data(), msg.length());
    tmp_client = nullptr;
}

void QT_MainWindow::send_acc_publickey_to_client(QString client_index, char typenumber)
{
    QTcpSocket *tmp_client = this->TcpSockets_List.value(client_index);
    QString tmp_acc_publickey = QString::fromStdString(this->acc_ptr->public_key.get_str(16));
    int msg_size = tmp_acc_publickey.size();
    tmp_client->write((char *)&msg_size, 4);
    tmp_client->write(&typenumber, 1);
    tmp_client->write(tmp_acc_publickey.toUtf8().data(), tmp_acc_publickey.length());
    tmp_client = nullptr;

}

void QT_MainWindow::send_file_info_to_client(QString client_index, char typenumber, QString filepath)
{

    QFileInfo info(filepath);
    if (info.exists())
    {
        this->send_file_path = filepath;
        this->send_file_name = info.fileName();
        this->send_file_size = info.size();
        this->current_client_index = client_index;
        this->send_part_file_size = 0;
        file.setFileName(filepath);
        bool isok = file.open(QIODevice::ReadOnly);
        if (isok == false)
        {
            qDebug() << "file open false";
        }
        QTcpSocket *tmp_client = this->TcpSockets_List.value(client_index);

        QString file_pid = this->qt_filechoose.get_file_pid(filepath);
        QString file_hash = this->qt_filechoose.get_file_hash(filepath);

        //        Process *process1 = new Process(file_pid.toStdString(), this->k, this->Ppubhex.toStdString());
        //生成部分私钥
        std::string file_pid_std = file_pid.toStdString();
        std::vector<std::string> partial_key = kgc_ptr->GeneratePartialKey(file_pid_std);
        QString partial_key1 = QString::fromStdString(partial_key[0]);
        QString partial_key2 = QString::fromStdString(partial_key[1]);
        //生成累加器证据值
        mpz_class tmp_pid;
        tmp_pid.set_str(file_pid_std, 16);
        QString file_acc_wit = QString::fromStdString(acc_ptr->witness_generate_by_pid(tmp_pid).get_str(16));


        QString client_ip = client_index.section(":", 0, 0);
        QString client_port = client_index.section(":", 1, 1);



        QString str = QString("%1##%2##%3##%4##%5##%6##%7").arg(this->send_file_name).arg(send_file_size)
                      .arg(file_pid).arg(file_hash).arg(partial_key1).arg(partial_key2).arg(file_acc_wit);
        int msg_size = str.size();
        tmp_client->write((char *)&msg_size, 4);
        tmp_client->write(&typenumber, 1);
        quint64 sendlen = tmp_client->write(str.toUtf8().data(), str.length());


        //文件信息写入json
        if (loader::write_json_host_file(this->client_json_path.toStdString(), send_file_name.toStdString(),
                                         filepath.toStdString(), file_hash.toStdString(),
                                         file_pid.toStdString(), client_ip.toStdString(),
                                         client_port.toStdString(), partial_key1.toStdString(), partial_key2.toStdString()))
        {
            qDebug() << "file write to client_file.json";
        }



        if (sendlen > 0)
        {
            mytimer.start(50);
        }
        else
        {
            qDebug() << "send msg failue";
            file.close();
        }
    }
}

void QT_MainWindow::send_file_body_to_client()
{
    this->mytimer.stop();
    quint64 len = 0;
    char typenumber = '5';

    do
    {
        char buf[4096] = {0};
        len = file.read(buf, sizeof(buf));
        QTcpSocket *tmp_client = this->TcpSockets_List.value(current_client_index);
        tmp_client->write((char *)&len, 4);
        tmp_client->write(&typenumber, 1);
        len = tmp_client->write(buf, len);
        this->send_part_file_size += len;
        tmp_client->waitForBytesWritten(100);
        qDebug() << len;
    } while (len > 0);
    if (this->send_part_file_size == this->send_file_size)
    {
        qDebug() << send_file_path << "has send to client";
    }

}

void QT_MainWindow::receive_common_msg_from_client(QString client_ip, quint16 client_port)
{
    QString msg =  QString( this->Serialize_receive_dataArray);
    emit signal_send_commonmsg_to_sockpage(client_ip, client_port, "0:" + msg + "\n");
    this->Serialize_receive_dataArray.clear();
    this->Serialize_receive_dataArray.resize(0);

}


//初始化KGC和累加器
void QT_MainWindow::slot_initialzeKGC_from_initialpage()
{

    //acc
    this->acc_ptr = new Accumulator(_KEY_LEN);
    this->acc_ptr->setup();
    QString acc_str = QString::fromStdString( acc_ptr->print_params());
    emit signal_message_to_initialpage("【接入认证模块】 初始化成功\n【认证模块】 初始化成功\n【系统参数】" + acc_str);

    // kgc
    this->kgc_ptr = new KGC(this->k);
    this->Ppubhex = QString::fromStdString( point2hex(kgc_ptr->ec_group, kgc_ptr->P_pub));
    emit signal_message_to_initialpage("【KGC】 初始化成功\n【系统参数】\n" + Ppubhex);

    this->ui->btn_fileissuance->setEnabled(true);


    std::string kgc_s = this->kgc_ptr->get_s();
    std::string kgc_q = this->kgc_ptr->get_q();
    std::string acc_publickey = this->acc_ptr->public_key.get_str();
    std::string acc_serect_partone = this->acc_ptr->secret_key.first.get_str();
    std::string acc_serect_parttwo = this->acc_ptr->secret_key.second.get_str();
    std::string acc_init = this->acc_ptr->acc_init.get_str();
    if (loader::write_json_accumulator_kgc(acc_kgc_path.toStdString(), acc_publickey, acc_serect_partone, acc_serect_parttwo, acc_init, kgc_s, kgc_q, k))
    {
        emit signal_message_to_initialpage("参数写入" + this->acc_kgc_path + "\n");
    }
    else
    {
        emit signal_message_to_initialpage("参数写入" + this->acc_kgc_path + "失败\n");
    }

}
//
void QT_MainWindow::slot_resetKGC_from_initialpage()
{
    delete this->kgc_ptr;
    delete this->acc_ptr;
    emit signal_message_to_initialpage("【重置成功】请重新初始化\n");
    this->ui->btn_fileissuance->setEnabled(false);
}

void QT_MainWindow::slot_startlistenning_from_socketpage(QString ip, quint16 port)
{
    this->listening_ip = new QHostAddress();
    if (!listening_ip->setAddress(ip))
    {
        //用这个函数判断IP地址是否可以被正确解析
        //不能被正确解析就发出错误信号
        QMessageBox::warning(this, "warning", "ip error");
        return;
    }

    this->server->listen(*(this->listening_ip), port);
}

void QT_MainWindow::slot_newconnection_from_client()
{
    //获取客户端连接
    this->socket = server->nextPendingConnection();
    if (this->socket == nullptr)
    {
        return;
    }
    QString client_ip = this->socket->peerAddress().toString();
    quint16 client_port = this->socket->peerPort();
    //    把新进来的客户端存入列表
    this->TcpSockets_List.insert(client_ip + ":" + QString::number(client_port), socket);
    this->client_list.append(client_ip + ":" + QString::number(client_port));

    //在socketserver文本框中显示新链接进来的客户端的IP地址
    emit signal_clientmsg_to_socketpage( client_ip, client_port);
    //连接QTcpSocket的信号槽，以读取新数据
    QObject::connect(socket, &QTcpSocket::readyRead, this, &QT_MainWindow::slot_readdata_from_client);
    QObject::connect(socket, &QTcpSocket::disconnected, this, &QT_MainWindow::slot_dissconect_from_client);
    this->Client_num++;

    emit signal_change_client_to_fileissuancepage(this->client_list);

}

void QT_MainWindow::slot_dissconect_from_client()
{
    QTcpSocket *client = static_cast<QTcpSocket *>(sender());
    QString client_ip = client->peerAddress().toString();
    quint16 client_port = client->peerPort();
    qDebug() << "receive disconnect!" << client_ip << client_port;
    emit signal_disconnect_clientmsg_to_socketpage(client_ip, client_port);

    if (this->TcpSockets_List.contains(client_ip + ":" + QString::number( client_port)))
    {
        this->TcpSockets_List.value(client_ip + ":" + QString::number(client_port))->deleteLater();
        this->TcpSockets_List.remove(client_ip + ":" + QString::number( client_port));
    }
    this->Client_num --;
    client = nullptr;
    this->client_list.removeAll(client_ip + ":" + QString::number( client_port));


    emit signal_change_client_to_fileissuancepage(this->client_list);

}

void QT_MainWindow::slot_readdata_from_client()
{
    QTcpSocket *tmp_client = static_cast<QTcpSocket *>(sender());
    QString client_ip = this->socket->peerAddress().toString();
    quint16 client_port = this->socket->peerPort();
    int count = 0;
    int total = 0;
    char typenumber;
    while (1)
    {
        if (count == 0)
        {
            tmp_client->read((char *)&total, 4);
            tmp_client->read(&typenumber, 1);
        }
        QByteArray all = tmp_client->readAll();
        count += all.size();
        this->Serialize_receive_dataArray += all;
        if (count == total)
        {

            emit signal_deal_receivedmsg_to_mainwindow(client_ip, client_port, typenumber);
            return;
        }
        else if (count  > total)
        {
            emit(signal_send_errormsg_to_socketpage(client_ip, client_port, "receive error: count > total\n"));
            tmp_client->close();
            tmp_client->deleteLater();
            return;
        }

    }
}

void QT_MainWindow::slot_deal_receive_data_from_client(QString client_ip, quint16 client_port, char typenumber)
{
    switch (typenumber)
    {
        case '0':
            receive_common_msg_from_client(client_ip, client_port);
            break;
        default:
            break;
    }



}

void QT_MainWindow::slot_send_msg_to_client_from_socketpage(QString client_index, char typenumber, QString msg)
{
    if (this->TcpSockets_List.contains(client_index))
    {
        switch (typenumber)
        {
            case '0':
                send_comment_msg_to_client(client_index, typenumber, msg);
                break;
            case '1':
                send_kgc_parameter_to_client(client_index, typenumber);
                break;
            case '3':
                send_file_info_to_client(client_index, typenumber, msg);
                break;
            case'5':
                    send_file_body_to_client();
                break;
            case '7':
                send_acc_publickey_to_client(client_index, typenumber);
                break;

            default:
                break;
        }
    }

}

void QT_MainWindow::slot_stop_listening_from_socketpage()
{
    //循环删除tcpsocket_list中的内容
    QMapIterator<QString, QTcpSocket *> itor(this->TcpSockets_List);
    while (itor.hasNext())
    {
        itor.next();
        itor.value()->close();
        itor.value()->deleteLater();
        //        itor.value()=nullptr;
        this->TcpSockets_List.remove(itor.key());
    }
    this->server->close();
    this->client_list.clear();

}

void QT_MainWindow::slot_send_file_to_client(QString client_id, QList<QString> file_lsit)
{
    for (int i = 0; i < file_lsit.size(); ++i)
    {

        slot_send_msg_to_client_from_socketpage(client_id, '3', file_lsit[i]);
        this->sleep(3000);

    }
    QMessageBox::information(this, "success", "files has send to client");



}

void QT_MainWindow::slot_calculate_file_pid_fromfilechoosepage(QStringList file_path_list)
{

    QMap<QString, QString> file_pid_map;
    for (int i = 0; i < file_path_list.size(); i++)
    {
        QString filepath = file_path_list[i];
        QFileInfo info = QFileInfo(filepath);
        //计算文件hash和身份
        QString filename = info.fileName();
        std::string  md5 = loader::compute_file_md5(filepath.toStdString());
        std::string pid = loader::md5_to_entityId(md5);
        QString qmd5 = QString::fromStdString(md5);
        QString qpid = QString::fromStdString(pid);

        if (loader::write_json_single(this->savepath.toStdString(), filename.toStdString(), filepath.toStdString(), md5, pid))
        {
            file_pid_map.insert(filepath + ":md5", qpid);
            file_pid_map.insert(filepath + ":pid", qpid);

            //加入密码学累加器
            mpz_class tmp_pid;
            tmp_pid.set_str(pid, 16);
            this->acc_ptr->add_member(tmp_pid);
        }
        else
        {
            QMessageBox::information(this, "提示", "警告 " + filename + " 计算失败");
            return;
        }


    }
    this->qt_filechoose.slot_update_filemap(file_pid_map);
}
