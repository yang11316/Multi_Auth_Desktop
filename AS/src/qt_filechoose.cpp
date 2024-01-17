#include "qt_filechoose.h"
#include "ui_qt_filechoose.h"


QT_FileChoose::QT_FileChoose(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QT_FileChoose)
{
    ui->setupUi(this);
    //读取json，记录已认证文件
    check_file_form_json();


    connect(this->ui->btn_filechose, &QPushButton::clicked, this, &QT_FileChoose::slot_filechosebtn_clicked);
    connect(this->ui->btn_delete, &QPushButton::clicked, this, &QT_FileChoose::slot_fliedeltebtn_clicked);
    connect(this->ui->listWidget, &QListWidget::itemClicked, this, &QT_FileChoose::slot_listwidget_clicked);
    connect(this->ui->btn_clear, &QPushButton::clicked, this, &QT_FileChoose::slot_clearbtn_clicked);
    connect(this->ui->btn_calculate, &QPushButton::clicked, this, &QT_FileChoose::slot_calculatebtn_clicked);
}

QT_FileChoose::~QT_FileChoose()
{
    delete ui;
}

void QT_FileChoose::check_file_form_json()
{

    std::ifstream ifs;
    ifs.open(savepath.toStdString());
    if (ifs.good())
    {
        std::vector<file_struct> vector_file_json;
        ifs.close();
        vector_file_json = loader::read_file_json(savepath.toStdString());
        if (vector_file_json.size() != 0 )
        {
            for (unsigned i = 0 ; i < vector_file_json.size() ; i++)
            {
                std::string path = vector_file_json[i].file_path;
                std::string  md5 = loader::compute_file_md5(path);
                if (md5 != vector_file_json[i].file_md5)
                {
                    continue;
                }

                filemap.insert(QString::fromStdString( vector_file_json[i].file_path), QString::fromStdString( vector_file_json[i].file_name));
                filemap.insert(QString::fromStdString( vector_file_json[i].file_path) + ":md5", QString::fromStdString( vector_file_json[i].file_md5));
                filemap.insert(QString::fromStdString( vector_file_json[i].file_path) + ":pid", QString::fromStdString( vector_file_json[i].file_pid));

                existing_file.insert(QString::fromStdString( vector_file_json[i].file_path));

                QCheckBox *checkbox = new QCheckBox;
                QListWidgetItem *pItem = new QListWidgetItem;
                pItem->setData(Qt::UserRole, QString::fromStdString( vector_file_json[i].file_path));
                pItem->setSizeHint(QSize(0, 30));
                checkbox->setText(QString::fromStdString( vector_file_json[i].file_name));
                ui->listWidget->addItem(pItem);
                ui->listWidget->setItemWidget(pItem, checkbox);
                checkbox->setCheckState(Qt::PartiallyChecked);
                connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(slot_check_box_state_changed(int)));

            }
        }
    }
}

QString QT_FileChoose::get_file_pid(QString filepath)
{
    return filemap.value(filepath + ":pid");
}

QString QT_FileChoose::get_file_hash(QString filepath)
{
    return filemap.value(filepath + ":md5");
}

void QT_FileChoose::slot_filechosebtn_clicked()
{
    QStringList filepaths = QFileDialog::getOpenFileNames(this, tr("Open File"),
                            QCoreApplication::applicationDirPath(), tr("*"));
    QStringList filenames;
    QFileInfo info;

    if (filepaths.isEmpty())
    {
        QMessageBox::warning(this, "警告", "请选择文件");
    }
    else
    {
        //获取文件名并判断是否有重复
        for (int i = 0; i < filepaths.size(); i++)
        {
            QString filename;
            info = QFileInfo(filepaths[i]);//获得每个路径的文件信息
            filename = info.fileName();

            if (filemap.contains(filepaths[i]) )
            {
                QMessageBox::warning(this, "警告", "请勿上传重复文件");
                return ;
            }
            filenames.append(filename);
        }
        //插入文件名
        for (int i = 0; i < filepaths.size(); i++)
        {
            filemap.insert(filepaths[i], filenames[i]);
            QCheckBox *checkbox = new QCheckBox;
            QListWidgetItem *pItem = new QListWidgetItem;
            pItem->setSizeHint(QSize(0, 30));
            //            pItem->setText(filenames[i]);
            /*

                pitem.data:
                    1.filepath
            */
            pItem->setData(Qt::UserRole, filepaths[i]);
            checkbox->setText(filenames[i]);
            ui->listWidget->addItem(pItem);
            ui->listWidget->setItemWidget(pItem, checkbox);

            connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(slot_checkboxstate_changed(int)));
        }
    }



}

void QT_FileChoose::slot_checkboxstate_changed(int )
{
    QListWidgetItem *item_now = ui->listWidget->currentItem();
    QCheckBox *checkbox_now = static_cast<QCheckBox *>(ui->listWidget->itemWidget(item_now));
    QString filepath_now = item_now->data(Qt::UserRole).toString();
    itemlist.clear();
    if (existing_file.contains(filepath_now))
    {
        checkbox_now->setCheckState(Qt::PartiallyChecked);
        qDebug() << itemlist;
        slot_listwidget_clicked();
        return;
    }
    else
    {

        for (int i = 0; i < ui->listWidget->count(); i++)
        {
            QListWidgetItem *item = ui->listWidget->item(i);

            QCheckBox *checkbox = static_cast<QCheckBox *>(ui->listWidget->itemWidget(item));
            if (checkbox->checkState() == 2)
            {
                QString filepath = item->data(Qt::UserRole).toString();
                itemlist.append(filepath);
            }
        }
        qDebug() << itemlist;
        slot_listwidget_clicked();
    }
}

void QT_FileChoose::slot_listwidget_clicked()
{
    ui->listwidget_fileinfo->clear();
    ui->edit_pid->clear();
    ui->edit_hash->clear();
    QListWidgetItem *item = ui->listWidget->currentItem();
    QString filepath = item->data(Qt::UserRole).toString();
    QString filename = filemap.value(filepath);


    QFileInfo finfo(filepath);
    QString file_path = "文件路径: " + filepath;
    QString file_size = "文件大小(字节): " + QString::number(finfo.size());
    QString file_birthtime = "文件创建时间: " + finfo.birthTime().toString("yyyy/MM/dd hh:mm:ss");
    QString file_lastmodified = "文件最近修改时间: " + finfo.lastModified().toString("yyyy/MM/dd hh:mm:ss");
    QString file_lastread = "文件最近读取时间: " + finfo.lastRead().toString("yyyy/MM/dd hh:mm:ss");
    QString file_houzhui = "文件后缀: " + finfo.suffix(); //完整的文件后缀后缀
    QString file_basename = "文件名: " + finfo.baseName(); //文件名;

    if (filemap.contains(filepath + ":md5"))
    {
        ui->btn_calculate->setDisabled(true);
        ui->edit_hash->setText(filemap.value(filepath + ":md5"));
        ui->edit_pid->setText(filemap.value(filepath + ":pid"));
    }
    else
    {
        ui->btn_calculate->setDisabled(false);
    }

    ui->listwidget_fileinfo->addItem(file_basename);
    ui->listwidget_fileinfo->addItem(file_houzhui);
    ui->listwidget_fileinfo->addItem(file_size);
    ui->listwidget_fileinfo->addItem(file_path);
    ui->listwidget_fileinfo->addItem(file_birthtime);
    ui->listwidget_fileinfo->addItem(file_lastmodified);
    ui->listwidget_fileinfo->addItem(file_lastread);
}

void QT_FileChoose::slot_fliedeltebtn_clicked()
{
    QListWidgetItem *item_now = ui->listWidget->currentItem();
    QCheckBox *checkbox_now = static_cast<QCheckBox *>(ui->listWidget->itemWidget(item_now));
    if (checkbox_now->checkState() == 1)
    {
        ui->edit_pid->clear();
        ui->edit_hash->clear();
    }

    for (int i = 0; i < ui->listWidget ->count(); i++)
    {
        QListWidgetItem *item = ui->listWidget->item(i);
        //将QWidget 转化为QCheckBox  获取第i个item 的控件
        QCheckBox *checkbox = static_cast<QCheckBox *>(ui->listWidget->itemWidget(item));
        if (checkbox->checkState() == 2)
        {
            ui->listWidget->takeItem(i);
            filemap.remove(item->data(Qt::UserRole).toString());

            if (filemap.contains(item->data(Qt::UserRole).toString() + ":md5"))
            {
                filemap.remove(item->data(Qt::UserRole).toString() + ":md5");
                filemap.remove(item->data(Qt::UserRole).toString() + ":pid");
            }

            i = i - 1;
        }
    }
    ui->listwidget_fileinfo->clear();
}

void QT_FileChoose::slot_clearbtn_clicked()
{
    QListWidgetItem *item = ui->listWidget->currentItem();
    QCheckBox *checkbox = static_cast<QCheckBox *>(ui->listWidget->itemWidget(item));



    // uncheckde : 0 ,partiallychecked : 1  ,checked : 2
    if (checkbox->checkState() == 1)
    {
        QString filepath = item->data(Qt::UserRole).toString();
        QString filename = filemap.value(filepath);

        //撤销
        if (loader::withdraw_file_json(savepath.toStdString(), filepath.toStdString()))
        {
            ui->edit_pid->clear();
            ui->edit_hash->clear();
            QMessageBox::information(this, "提示", "文件 " + filename + " 撤销授权");
        }
        else
        {
            QMessageBox::information(this, "警告", "文件 " + filename + " 撤销授权失败");
            return;
        }


        //解除checkbox

        existing_file.remove(filepath);

        if (filemap.contains(filepath + ":md5"))
        {
            filemap.remove(filepath + ":md5");
            filemap.remove(filepath + ":pid");
        }
        checkbox->setCheckState(Qt::Unchecked);
        checkbox->setTristate(false);
        //

    }
}

void QT_FileChoose::slot_update_filemap(QMap<QString, QString> sendmap)
{
    QListWidgetItem *item_now = ui->listWidget->currentItem();
    QCheckBox *checkbox_now = static_cast<QCheckBox *>(ui->listWidget->itemWidget(item_now));
    QString filename_now = checkbox_now->text();
    this->filemap.unite(sendmap);
    for (int i = 0; i < itemlist.size(); i++)
    {
        QString filepath = itemlist[i];
        QString filename = filemap.value(filepath);
        if (QString::compare(filename, filename_now, Qt::CaseSensitive) == 0)
        {
            ui->edit_hash->setText(this->filemap.value(filepath + ":md5"));
            ui->edit_pid->setText(this->filemap.value(filepath + ":pid"));

        }
        existing_file.insert(filepath);
    }
    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        QListWidgetItem *item = ui->listWidget->item(i);
        QCheckBox *checkbox = static_cast<QCheckBox *>(ui->listWidget->itemWidget(item));
        if (existing_file.contains(item->data(Qt::UserRole).toString()))
        {
            checkbox->setCheckState(Qt::PartiallyChecked);
        }
    }

}



void QT_FileChoose::slot_calculatebtn_clicked()
{
    emit signal_calculate_file_to_mainwindow(itemlist);
}




void QT_FileChoose::slot_check_box_state_changed(int)
{
    QListWidgetItem *item_now = ui->listWidget->currentItem();
    QCheckBox *checkbox_now =
        static_cast<QCheckBox *>(ui->listWidget->itemWidget(item_now));
    QString filepath_now = item_now->data(Qt::UserRole).toString();
    itemlist.clear();
    if (existing_file.contains(filepath_now))
    {
        checkbox_now->setCheckState(Qt::PartiallyChecked);
        qDebug() << itemlist;
        this->slot_listwidget_clicked();
        return;
    }
    else
    {
        for (int i = 0; i < ui->listWidget->count(); i++)
        {
            QListWidgetItem *item = ui->listWidget->item(i);

            QCheckBox *checkbox =
                static_cast<QCheckBox *>(ui->listWidget->itemWidget(item));
            if (checkbox->checkState() == 2)
            {
                QString filepath = item->data(Qt::UserRole).toString();
                itemlist.append(filepath);
            }
        }
        qDebug() << itemlist;
        this->slot_listwidget_clicked();
    }
}


