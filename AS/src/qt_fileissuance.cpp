#include "qt_fileissuance.h"
#include "ui_qt_fileissuance.h"

QT_Fileissuance::QT_Fileissuance(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QT_Fileissuance)
{
    ui->setupUi(this);

    connect(this->ui->widget_client, &QListWidget::itemClicked, this, &QT_Fileissuance::slot_click_client_widget);
    connect(this->ui->widget_file_2, &QListWidget::itemClicked, this, &QT_Fileissuance::slot_click_file_widget);
    connect(this->ui->btn_issuance_2, &QPushButton::clicked, this, &QT_Fileissuance::slot_click_issuancebtn);
}

QT_Fileissuance::~QT_Fileissuance()
{
    delete ui;
}



void QT_Fileissuance::slot_change_client_from_mainwindow(QList<QString> client_list)
{
    this->ui->widget_client->clear();
    for (int i = 0 ; i < client_list.size(); i++)
    {
        QListWidgetItem *pItem = new QListWidgetItem;
        pItem->setText(client_list[i]);
        this->ui->widget_client->addItem(pItem);
    }
}

void QT_Fileissuance::slot_click_client_widget()
{
    this->ui->widget_file_2->clear();

    QListWidgetItem *item = this->ui->widget_client->currentItem();
    QString client_id = item->data(Qt::UserRole).toString();
    filemap.clear();
    QString client_ip = client_id.section(":", 0, 0);
    QString client_port = client_id.section(":", 1, 1);
    std::vector<file_struct> this_file_struct = loader::read_file_json(file_json_path.toStdString());
    std::vector<host_file_struct> this_host_struct = loader::read_host_file_json(client_json_path.toStdString(), client_id.toStdString(),
            client_port.toStdString());
    if (this_file_struct.size() != 0 )
    {
        for (int i = 0 ; i < this_file_struct.size() ; i++)
        {
            QCheckBox *checkbox = new QCheckBox;
            QListWidgetItem *pItem = new QListWidgetItem;
            pItem->setData(Qt::UserRole, QString::fromStdString( this_file_struct[i].file_path));
            pItem->setSizeHint(QSize(0, 30));
            checkbox->setText(QString::fromStdString(this_file_struct[i].file_name));
            ui->widget_file_2->addItem(pItem);
            ui->widget_file_2->setItemWidget(pItem, checkbox);
            filemap.insert(QString::fromStdString(this_file_struct[i].file_path), QString::fromStdString(this_file_struct[i].file_name));
            filemap.insert(QString::fromStdString(this_file_struct[i].file_path) + ":md5", QString::fromStdString(this_file_struct[i].file_md5));
            filemap.insert(QString::fromStdString(this_file_struct[i].file_path) + ":pid", QString::fromStdString(this_file_struct[i].file_pid));
        }
        for (int j = 0; j < this_host_struct.size(); j++)
        {
            if (filemap.contains(QString::fromStdString( this_host_struct[j].file_path)))
            {
                if (!existing_file.contains(client_id + ":" + QString::fromStdString(this_host_struct[j].file_path)))
                {
                    existing_file.insert(client_id + ":" + QString::fromStdString(this_host_struct[j].file_path));
                }

            }
            else
            {
                if (loader::withdraw_host_file_json(client_json_path.toStdString(), this_host_struct[j].file_path, client_ip.toStdString(), client_port.toStdString()))
                {
                    existing_file.remove(client_id + ":" + QString::fromStdString(this_host_struct[j].file_path));
                    qDebug() << "success";
                }
                else
                {
                    qDebug() << "false";
                }
            }
        }

        qDebug() << existing_file;
        for (int k = 0; k < ui->widget_file_2->count(); k++)
        {
            QListWidgetItem *item = ui->widget_file_2->item(k);
            QString filepath = item->data(Qt::UserRole).toString();
            QCheckBox *checkbox = static_cast<QCheckBox *>(ui->widget_file_2->itemWidget(item));
            if (existing_file.contains(client_id + ":" + filepath))
            {
                checkbox->setCheckState(Qt::PartiallyChecked);
            }
            connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(slot_checkbox_state_change(int)));
        }
    }

}

void QT_Fileissuance::slot_click_file_widget()
{
    QListWidgetItem *item_now = ui->widget_file_2->currentItem();
    QCheckBox *checkbox_now = static_cast<QCheckBox *>(ui->widget_file_2->itemWidget(item_now));
    QString filepath_now = item_now->data(Qt::UserRole).toString();
    ui->edit_text_2->clear();

    QFileInfo finfo(filepath_now);

    QString file_path = "文件路径: " + filepath_now;
    QString file_houzhui = "文件后缀: " + finfo.suffix(); //完整的文件后缀后缀
    QString file_basename = "文件名: " + finfo.baseName(); //文件名
    ui->edit_text_2->append(file_path);
    ui->edit_text_2->append(file_basename);
    ui->edit_text_2->append(file_houzhui);

}

void QT_Fileissuance::slot_click_issuancebtn()
{
    QListWidgetItem *item_host = ui->widget_client->currentItem();
    QString client_id = item_host->text();

    QList<QString> file_list;
    for (int i = 0; i < itemlist.size(); i++)
    {
        file_list.append( itemlist[i]);
    }
    emit signal_issuance_file_to_client(client_id, file_list);
    for (int i = 0; i < ui->widget_client->count(); i++)
    {
        QListWidgetItem *item = ui->widget_client->item(i);
        QCheckBox *checkbox = static_cast<QCheckBox *>(ui->widget_client->itemWidget(item));
        if (existing_file.contains(client_id + ":" + item->data(Qt::UserRole).toString()))
        {
            checkbox->setCheckState(Qt::PartiallyChecked);
        }
    }
}

void QT_Fileissuance::slot_checkbox_state_change(int)
{
    QListWidgetItem *item_now = ui->widget_file_2->currentItem();
    QListWidgetItem *host_item_now = ui->widget_client->currentItem();
    QString host_name = host_item_now->text();
    QCheckBox *checkbox_now = static_cast<QCheckBox *>(ui->widget_file_2->itemWidget(item_now));
    QString filepath_now = item_now->data(Qt::UserRole).toString();

    if (existing_file.contains(host_name + ":" + filepath_now))
    {
        checkbox_now->setCheckState(Qt::PartiallyChecked);
        qDebug() << itemlist;
    }
    else
    {
        itemlist.clear();
        for (int i = 0; i < ui->widget_file_2->count(); i++)
        {
            QListWidgetItem *item = ui->widget_file_2->item(i);
            QCheckBox *checkbox = static_cast<QCheckBox *>(ui->widget_file_2->itemWidget(item));
            if (checkbox->checkState() == 2)
            {
                QString filepath = item->data(Qt::UserRole).toString();
                itemlist.append(filepath);
                qDebug() << itemlist;
                qDebug() << filemap;
            }
        }

    }
    slot_click_file_widget();
}
