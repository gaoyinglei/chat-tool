#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QDialog>
#include "QPushButton"
#include "QLabel"
#include "QLineEdit"

namespace Ui {
class loginDlg;
}

class loginDlg : public QDialog
{
    Q_OBJECT

public:
    explicit loginDlg(QWidget *parent = 0);
    ~loginDlg();
    QLineEdit *nameEdit;
    QLineEdit *address;
private:
    Ui::loginDlg *ui;
    QLabel *nickName; 
    QLabel *IP;
    QPushButton *login;

private slots:
    void loginSuccess();
};

#endif // LOGINDLG_H
