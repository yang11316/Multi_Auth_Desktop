#include "qt_filemanage.h"
#include "ui_qt_filemanage.h"

QT_FileManage::QT_FileManage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QT_FileManage)
{
    ui->setupUi(this);



    slot_update_filewidget();

    connect(this->ui->key_btn, &QPushButton::clicked, this, &QT_FileManage::slot_keybtn_clicked);
    connect(ui->file_lsitwidget, &QListWidget::itemClicked, this,
            &QT_FileManage::slot_filewidget_clicked);
}

QT_FileManage::~QT_FileManage()
{
    delete ui;
}

void QT_FileManage::slot_update_filewidget()
{
    std::ifstream ifs;
    ifs.open(this->save_file_path.toStdString());
    if (ifs.good())
    {
        std::vector<file_struct> vector_file_json;
        ifs.close();
        vector_file_json = loader::read_file_json(this->save_file_path.toStdString());
        if (vector_file_json.size() != 0)
        {
            this->ui->file_lsitwidget->clear();
            this->pid_map.clear();
            this->partical1_map.clear();
            this->partical2_map.clear();
            this->existing_file.clear();


            for (int i = 0; i < vector_file_json.size(); i++)
            {
                std::string file_path = vector_file_json[i].file_path;
                std::string file_name = vector_file_json[i].file_name;
                std::string file_pid = vector_file_json[i].file_pid;
                std::string file_partial1 = vector_file_json[i].partial_key1;
                std::string file_partial2 = vector_file_json[i].partial_key2;
                //添加累加器
                this->pid_map.insert(
                    QString::fromStdString(file_path) + ":pid",
                    QString::fromStdString(file_pid)
                );

                this->partical1_map.insert(
                    QString::fromStdString(file_path) + ":partial_key1",
                    QString::fromStdString(file_partial1));
                this->partical2_map.insert(
                    QString::fromStdString(file_path) + ":partial_key2",
                    QString::fromStdString(file_partial2));
                existing_file.insert(QString::fromStdString(file_path));

                QListWidgetItem *pItem = new QListWidgetItem;
                pItem->setData(
                    Qt::UserRole,
                    QString::fromStdString(vector_file_json[i].file_path));
                pItem->setSizeHint(QSize(0, 30));
                pItem->setText(QString::fromStdString(file_name));
            }

        }


    }
}

void QT_FileManage::slot_filewidget_clicked()
{
    QListWidgetItem *item = ui->file_lsitwidget->currentItem();
    if (item != nullptr)
    {
        ui->file_edit->clear();
        ui->key_btn->setEnabled(true);
        ui->send_btn->setEnabled(false);
        QString filepath = item->data(Qt::UserRole).toString();
        QString filename = item->text();
        QFileInfo finfo(filepath);
        if (finfo.exists())
        {
            QString file_path = "文件路径: " + filepath + "\n";
            QString file_size = "文件大小(字节): " + QString::number(finfo.size()) + "\n";
            QString file_birthtime =
                "文件创建时间: " + finfo.birthTime().toString("yyyy/MM/dd hh:mm:ss") + "\n";
            QString file_lastmodified =
                "文件最近修改时间: " +
                finfo.lastModified().toString("yyyy/MM/dd hh:mm:ss") + "\n";
            QString file_lastread =
                "文件最近读取时间: " + finfo.lastRead().toString("yyyy/MM/dd hh:mm:ss") + "\n";
            QString file_houzhui = "文件后缀: " + finfo.suffix() + "\n"; // 完整的文件后缀后缀
            QString file_basename = "文件名: " + finfo.baseName() + "\n"; // 文件名;

            ui->file_edit->appendPlainText(file_basename);
            ui->file_edit->appendPlainText(file_houzhui);
            ui->file_edit->appendPlainText(file_size);
            ui->file_edit->appendPlainText(file_path);
            ui->file_edit->appendPlainText(file_birthtime);
            ui->file_edit->appendPlainText(file_lastmodified);
            ui->file_edit->appendPlainText(file_lastread);

        }
    }
}

void QT_FileManage::slot_keybtn_clicked()
{
    QListWidgetItem *item = ui->file_lsitwidget->currentItem();
    if (item != nullptr)
    {
        QString file_path = item->data(Qt::UserRole).toString();
        emit signal_cal_file_fullkey_to_mainwindow(file_path);
    }
}
