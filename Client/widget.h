#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "QListWidget"
#include "chat.h"
#include "QTcpSocket"
#include "QUdpSocket"

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
    QListWidget *userWidget;

    QList<Chat*> chatting;
    QList<QString> userNameList;
    QList<int> userIdList;

    QTcpSocket *tcpSocket;
    QUdpSocket *udpSocket;
    QHostAddress address;
    quint16 tcpPort;
    quint16 udpPort;

    QString nickName;
    QString filePath;

private slots:
    void showChat(QListWidgetItem*);
    void closeEvent(QCloseEvent*);
    void closeClient(QAbstractSocket::SocketError);
    void udpConnect();
    void udpSend(int, QString);
    void udpRecv();
    void tcpSend(int, QString, QByteArray);
    void TcpRecv();
};

#endif // WIDGET_H
