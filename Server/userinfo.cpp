#include "userinfo.h"
#include "QDataStream"

userInfo::userInfo(QObject *parent) : QObject(parent)
{
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(tcpRecv()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(tcpDisconnected()));
}

void userInfo::tcpRecv()
{
    QDataStream in(tcpSocket);
    int toId;
    QString fileName;
    QByteArray fileData;
    in >> toId >> fileName >> fileData;

    emit tcpSendFile(this->userId, toId, fileName, fileData);
}

void userInfo::tcpDisconnected()
{
    emit removeTcpSocket(this->userId);
}
