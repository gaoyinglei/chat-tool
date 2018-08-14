#include "chat.h"
#include "ui_chat.h"
#include "QDateTime"
#include "QFileDialog"

Chat::Chat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);
    this->setFixedSize(500, 400);

    recvText = new QTextEdit(this);
    recvText->setGeometry(20, 10, 460, 200);
    recvText->setReadOnly(true);
    sendText = new QTextEdit(this);
    sendText->setGeometry(20, 220, 460, 120);

    fileBtn = new QPushButton(QStringLiteral("发送文件"), this);
    fileBtn->setGeometry(90, 355, 80, 30);
    msgBtn = new QPushButton(QStringLiteral("发送消息"), this);
    msgBtn->setGeometry(330, 355, 80, 30);
    connect(fileBtn, SIGNAL(clicked(bool)), this, SLOT(sendFile()));
    connect(msgBtn, SIGNAL(clicked(bool)), this, SLOT(sendMessage()));
}

Chat::~Chat()
{
    delete ui;
}

void Chat::sendMessage()
{
    QString msg = sendText->toPlainText();
    if(msg != "")
    {
        sendText->clear();

        QDateTime time = QDateTime::currentDateTime();
        QString str = QStringLiteral("我 %1").arg(time.toString("yyyy-MM-dd hh:mm:ss"));
        recvText->append(str);
        recvText->append(msg);
        recvText->append("");

        emit udpSend(this->userId, msg);
    }
    sendText->setFocus();
}

void Chat::sendFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, QStringLiteral("选择文件"));
    if (!filePath.isEmpty())
    {
        QString fileName = QFileInfo(filePath).fileName();
        QFile file(filePath);
        file.open(QIODevice::ReadOnly);
        QByteArray fileData = file.readAll() ;
        file.close();

        QDateTime time = QDateTime::currentDateTime();
        QString str = QStringLiteral("%1 我发送文件 %2").arg(time.toString("yyyy-MM-dd hh:mm:ss")).arg(fileName);
        recvText->append(str);
        emit tcpSend(this->userId, fileName, fileData);
    }
}
