#ifndef QT_CONNECTCLIENT_H
#define QT_CONNECTCLIENT_H

#include <QWidget>

namespace Ui {
class QT_ConnectClient;
}

class QT_ConnectClient : public QWidget
{
    Q_OBJECT

public:
    explicit QT_ConnectClient(QWidget *parent = nullptr);
    ~QT_ConnectClient();

private:
    Ui::QT_ConnectClient *ui;
};

#endif // QT_CONNECTCLIENT_H
