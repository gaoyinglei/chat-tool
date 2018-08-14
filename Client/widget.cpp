#include "widget.h"
#include "ui_widget.h"
#include "QDesktopWidget"
#include "logindlg.h"
#include "QDateTime"
#include "QFileDialog"
#include "QTimer"
#include "QMessageBox"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    loginDlg *log = new loginDlg();

    if(log->exec() != QDialog::Accepted)
    {
        exit(0);
    }

    nickName = log->nameEdit->text();
    this->setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    this->setWindowTitle(nickName);
    QDesktopWidget* desktop = QApplication::desktop();
    this->setGeometry(desktop->width()/4*3, 100, 280, 520);

    filePath = "D:\\";

    userWidget = new QListWidget(this);
    QString userWidgetStyle = "QListWidget{font-size: 20px;}";
    userWidget->setStyleSheet(userWidgetStyle);
    userWidget->setGeometry(10, 10, 260, 500);

    connect(userWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(showChat(QListWidgetItem*)));

    tcpSocket = new QTcpSocket(this);
    udpSocket = new QUdpSocket(this);

    address.setAddress(log->address->text());
    tcpPort = 22222;
    udpPort = 33333;

    tcpSocket->connectToHost(address, tcpPort);
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(udpConnect()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(closeClient(QAbstractSocket::SocketError)));

    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(TcpRecv()));
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(udpRecv()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::showChat(QListWidgetItem* item)
{
    int i = 0;
    while(i < chatting.count())
    {
        if(chatting.at(i)->otherName == item->text()) break;
        i++;
    }
    if(chatting.at(i)->isHidden())
    {
        chatting.at(i)->show();
        item->setTextColor(QColor(0, 0, 0));
    }
    chatting.at(i)->raise();
    chatting.at(i)->sendText->setFocus();

}

void Widget::closeEvent(QCloseEvent *)
{
    for(int i = 0; i < chatting.count(); i++)
    {
        chatting.at(i)->close();
    }
}

void Widget::closeClient(QAbstractSocket::SocketError error)
{
    if(error == 0)
    {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("连接被拒绝"), QMessageBox::Yes);
    }
    else if(error == 1)
    {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("服务器关闭连接"), QMessageBox::Yes);
    }
    else if(error == 2)
    {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("未找到服务器"), QMessageBox::Yes);
    }
    else if(error == 3)
    {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("socket操作失败"), QMessageBox::Yes);
    }
    else if(error == 4)
    {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("系统资源不足"), QMessageBox::Yes);
    }
    else if(error == 5)
    {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("socket超时"), QMessageBox::Yes);
    }
    else if(error == 7)
    {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("网络错误"), QMessageBox::Yes);
    }
    else
    {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("其他错误"), QMessageBox::Yes);
    }
    exit(0);
}

void Widget::udpConnect()
{
    QByteArray datagram;
    QDataStream out(&datagram, QIODevice::WriteOnly);
    out << 10000 << nickName;
    udpSocket->writeDatagram(datagram, address, udpPort);
}

void Widget::udpSend(int userId, QString msg)
{
    QByteArray datagram;
    QDataStream out(&datagram, QIODevice::WriteOnly);
    out << userId << msg;
    udpSocket->writeDatagram(datagram, address, udpPort);
}

void Widget::udpRecv()
{
    QByteArray datagram;
    int fromId;
    QString fromMsg;

    while(udpSocket->hasPendingDatagrams())
    {
       datagram.resize(udpSocket->pendingDatagramSize());
       udpSocket->readDatagram(datagram.data(),datagram.size(), &address, &udpPort);
     }

    QDataStream in(&datagram, QIODevice::ReadOnly);
    in >> fromId >> fromMsg;

    int i = 0;
    while(i < chatting.count())
    {
        if(chatting.at(i)->userId == fromId) break;
        i++;
    }

    QDateTime time = QDateTime::currentDateTime();
    QString str = QString("%1 %2").arg(chatting.at(i)->otherName).arg(time.toString("yyyy-MM-dd hh:mm:ss"));
    chatting.at(i)->recvText->append(str);
    chatting.at(i)->recvText->append(fromMsg);
    chatting.at(i)->recvText->append("");

    if(chatting.at(i)->isHidden())
    {
        userWidget->item(i)->setTextColor(QColor(255, 0, 0));
    }
}

void Widget::tcpSend(int userId, QString fileName, QByteArray fileData)
{
    QByteArray datagram;
    QDataStream out(&datagram, QIODevice::WriteOnly);
    out << userId << fileName << fileData;
    tcpSocket->write(datagram);
}

void Widget::TcpRecv()
{
    QDataStream in(tcpSocket);
    int flag;
    in >> flag;
    if(flag == 1)
    {
        in >> userIdList >> userNameList;
        for(int i = 0; i < userIdList.count(); i++)
        {
            Chat *chattingOne = new Chat();
            chattingOne->otherName = userNameList.at(i);
            chattingOne->nickName = nickName;
            chattingOne->userId = userIdList.at(i);

            userWidget->addItem(chattingOne->otherName);
            userWidget->item(i)->setTextAlignment(Qt::AlignHCenter);

            QString str = QStringLiteral("与 %1 %2").arg(chattingOne->otherName).arg(QStringLiteral("聊天"));
            chattingOne->setWindowTitle(str);

            connect(chattingOne, SIGNAL(udpSend(int, QString)), this, SLOT(udpSend(int, QString)));
            connect(chattingOne, SIGNAL(tcpSend(int, QString, QByteArray)), this, SLOT(tcpSend(int, QString, QByteArray)));

            chatting.append(chattingOne);
        }
    }
    else if(flag == 2)
    {
        int newUserId;
        QString newUserName;
        in >> newUserId >> newUserName;

        Chat *chattingOne = new Chat();

        chattingOne->otherName = newUserName;
        chattingOne->nickName = nickName;
        chattingOne->userId = newUserId;
        userWidget->addItem(chattingOne->otherName);
        userWidget->item(userWidget->count()-1)->setTextAlignment(Qt::AlignHCenter);
        QString title = QStringLiteral("与 %1 %2").arg(chattingOne->otherName).arg(QStringLiteral("聊天"));
        chattingOne->setWindowTitle(title);

        connect(chattingOne, SIGNAL(udpSend(int, QString)), this, SLOT(udpSend(int, QString)));
        connect(chattingOne, SIGNAL(tcpSend(int, QString, QByteArray)), this, SLOT(tcpSend(int, QString, QByteArray)));

        chatting.append(chattingOne);
    }
    else if(flag == 3)
    {
        int removeUserId;
        in >> removeUserId;

        int i = 0;
        while(i < chatting.count())
        {
            if(chatting.at(i)->userId == removeUserId) break;
            i++;
        }

        chatting.at(i)->close();
        chatting.removeAt(i);
        userWidget->takeItem(i);
    }
    else
    {
        int fromId;
        QString fileName;
        QByteArray fileData;
        in >> fromId >> fileName >> fileData;

        int i = 0;
        while(i < chatting.count())
        {
            if(chatting.at(i)->userId == fromId) break;
            i++;
        }

        QString path = QString("%1%2").arg(filePath).arg(fileName);
        QFile file(path);
        file.open(QIODevice::WriteOnly);
        file.write(fileData);
        file.close();

        QDateTime time = QDateTime::currentDateTime();
        QString str = QStringLiteral("%1 %2 发送文件 %3").arg(time.toString("yyyy-MM-dd hh:mm:ss"))
                .arg(chatting.at(i)->otherName).arg(fileName);
        chatting.at(i)->recvText->append(str);
        chatting.at(i)->recvText->append("");

        if(chatting.at(i)->isHidden())
        {
            userWidget->item(i)->setTextColor(QColor(255, 0, 0));
        }
    }
}
