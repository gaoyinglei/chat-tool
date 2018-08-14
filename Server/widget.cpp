#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setFixedSize(500, 400);
    this->setWindowTitle(QStringLiteral("服务器"));

    information = new QListWidget(this);
    information->setGeometry(10, 10, 480, 300);

    btnStyle = "QPushButton{font-size: 30px;}";
    start = new QPushButton(QStringLiteral("开始"), this);
    start->setStyleSheet(btnStyle);
    start->setGeometry(50, 330, 150, 60);
    end = new QPushButton(QStringLiteral("结束"), this);
    end->setStyleSheet(btnStyle);
    end->setGeometry(300, 330, 150, 60);
    end->setEnabled(false);

    connect(start, SIGNAL(clicked(bool)), this, SLOT(startService()));
    connect(end, SIGNAL(clicked(bool)), this, SLOT(endService()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::startService()
{
    tcpServer = new QTcpServer(this);
    udpSocket = new QUdpSocket(this);
    tcpPort = 22222;
    udpPort = 33333;
    newId = 1;

    tcpServer->listen(QHostAddress::Any, tcpPort);
    udpSocket->bind(QHostAddress::Any, udpPort, QUdpSocket::ShareAddress);

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(tcpConnect()));
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(udpRecv()));

    start->setEnabled(false);
    end->setEnabled(true);
    information->addItem(QStringLiteral("服务器已开启"));
}

void Widget::endService()
{
    delete tcpServer;
    delete udpSocket;
    userList.clear();
    userIdList.clear();
    userNameList.clear();
    start->setEnabled(true);
    end->setEnabled(false);
    information->addItem(QStringLiteral("服务器已关闭"));
}

void Widget::tcpConnect()
{
    userInfo *newUser = new userInfo();
    newUser->tcpSocket->setSocketDescriptor(tcpServer->nextPendingConnection()->socketDescriptor());

    connect(newUser, SIGNAL(tcpSendFile(int, int, QString, QByteArray)), this, SLOT(tcpSendFile(int, int, QString, QByteArray)));
    connect(newUser, SIGNAL(removeTcpSocket(int)), this, SLOT(removeUser(int)));

    QByteArray datagram;
    QDataStream out(&datagram, QIODevice::WriteOnly);
    out << 1 << userIdList << userNameList;
    newUser->tcpSocket->write(datagram);
    userList.append(newUser);
}

void Widget::tcpSendFile(int fromId, int toId, QString fileName, QByteArray fileData)
{
    int i = 0;
    while(i < userList.count())
    {
        if(userList.at(i)->userId == toId) break;
        i++;
    }
    QByteArray datagram;
    QDataStream out(&datagram, QIODevice::WriteOnly);
    out << 4 << fromId << fileName << fileData;
    userList.at(i)->tcpSocket->write(datagram);
}

void Widget::udpRecv()
{
    QByteArray datagram;
    QHostAddress address;
    quint16 port;

    while(udpSocket->hasPendingDatagrams())//拥有等待的数据报
    {
       datagram.resize(udpSocket->pendingDatagramSize());//让datagram的大小为等待处理的数据报的大小，这样才能接收到完整的数据
       udpSocket->readDatagram(datagram.data(),datagram.size(), &address, &port);//接收数据报，将其存放到datagram中
     }

    QDataStream in(&datagram, QIODevice::ReadOnly);
    QString msg;
    int toId;
    in >> toId >> msg;

    if(toId == 10000)
    {
        userInfo *newUser = userList.takeLast();

        userIdList.append(newId);
        userNameList.append(msg);

        newUser->userAddress = address;
        newUser->userPort = port;
        newUser->userId = newId;
        newUser->nickName = msg;

        for(int i = 0; i < userList.count(); i++)
        {
            QByteArray newUserInfo;
            QDataStream out(&newUserInfo, QIODevice::WriteOnly);
            out << 2 << newId << msg;
            userList.at(i)->tcpSocket->write(newUserInfo);
        }

        userList.append(newUser);

        QString str = QStringLiteral("用户 %1 连接服务器").arg(newUser->nickName);
        information->addItem(str);
        newId++;
    }
    else
    {
        int i = 0, j = 0;
        while(i < userList.count())
        {
            if(userList.at(i)->userId == toId) break;
            i++;
        }

        while(j < userList.count())
        {
            if(userList.at(j)->userAddress == address && userList.at(j)->userPort == port) break;
            j++;
        }

        QByteArray sendMsg;
        QDataStream out(&sendMsg, QIODevice::WriteOnly);
        out << userList.at(j)->userId << msg;
        udpSocket->writeDatagram(sendMsg, userList.at(i)->userAddress, userList.at(i)->userPort);
    }
}

void Widget::removeUser(int removeUserId)
{
    int i = userIdList.indexOf(removeUserId);

    QString str = QStringLiteral("用户 %1 断开连接").arg(userNameList.at(i));
    information->addItem(str);

    userList.removeAt(i);
    userIdList.removeAt(i);
    userNameList.removeAt(i);

    QByteArray removeUserInfo;
    QDataStream out(&removeUserInfo, QIODevice::WriteOnly);
    out << 3 << removeUserId ;

    for(int i = 0; i < userList.count(); i++)
    {
        userList.at(i)->tcpSocket->write(removeUserInfo);
    }
}
