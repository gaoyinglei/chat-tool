#ifndef CHAT_H
#define CHAT_H

#include <QWidget>
#include "QTextEdit"
#include "QPushButton"

namespace Ui {
class Chat;
}

class Chat : public QWidget
{
    Q_OBJECT

public:
    explicit Chat(QWidget *parent = 0);
    ~Chat();
    QString otherName;
    QString nickName;
    int userId;
    QTextEdit *sendText;
    QTextEdit *recvText;

private:
    Ui::Chat *ui;
    QPushButton *fileBtn;
    QPushButton *msgBtn;

signals:
    void udpSend(int, QString);
    void tcpSend(int, QString, QByteArray);
private slots:
    void sendMessage();
    void sendFile();
};

#endif // CHAT_H
