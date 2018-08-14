#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "QTcpServer"
#include "QTcpSocket"
#include "QUdpSocket"
#include "userinfo.h"
#include "QListWidget"
#include "QPushButton"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;

    QTcpServer *tcpServer;
    QUdpSocket *udpSocket;

    quint16 tcpPort;
    quint16 udpPort;

    QList<userInfo*> userList;
    QList<QString> userNameList;
    QList<int> userIdList;

    int newId;

    QListWidget *information;
    QPushButton *start;
    QPushButton *end;
    QString btnStyle;

signals:

private slots:
    void startService();
    void endService();
    void tcpConnect();
    void tcpSendFile(int, int, QString, QByteArray);
    void udpRecv();
    void removeUser(int);
};

#endif // WIDGET_H
