#include "logindlg.h"
#include "ui_logindlg.h"
#include "QMessageBox"

loginDlg::loginDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loginDlg)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowCloseButtonHint);
    this->setWindowTitle(QStringLiteral("登录界面"));
    this->setFixedSize(300, 150);
    nickName = new QLabel(QStringLiteral("昵称"), this);
    nickName->setGeometry(70, 20, 30, 20);
    nameEdit = new QLineEdit(this);
    nameEdit->setGeometry(110, 20, 100, 20);

    IP = new QLabel(QStringLiteral("地址"), this);
    IP->setGeometry(70, 70, 30, 20);
    address = new QLineEdit(this);
    address->setGeometry(110, 70, 100, 20);

    login = new QPushButton(QStringLiteral("登录"), this);
    login->setGeometry(115, 110, 70, 30);
    connect(login, SIGNAL(clicked(bool)), this, SLOT(loginSuccess()));
}

loginDlg::~loginDlg()
{
    delete ui;
}

void loginDlg::loginSuccess()
{
    if(nameEdit->text().trimmed() == "")
    {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("昵称不能为空"), QMessageBox::Yes);
    }
    else
    {
        accept();
    }
}
