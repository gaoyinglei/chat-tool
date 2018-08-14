#ifndef USERINFO_H
#define USERINFO_H

#include <QObject>
#include "QTcpSocket"
#include "QHostAddress"

class userInfo : public QObject
{
    Q_OBJECT
public:
    explicit userInfo(QObject *parent = 0);
    int userId;
    QString nickName;
    QHostAddress userAddress;
    quint16 userPort;
    QTcpSocket *tcpSocket;

signals:
    void tcpSendFile(int, int, QString, QByteArray);
    void removeTcpSocket(int);
private slots:
    void tcpRecv();
    void tcpDisconnected();
};

#endif // USERINFO_H
