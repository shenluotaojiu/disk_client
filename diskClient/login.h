#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QString>
#include "Packdef.h"
#include "kernel/tcpkernel.h"
#include <QMessageBox>
namespace Ui {
class login;
}

class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();
    void setKernel(iKernel* m_pkernel);

private slots:
    void on_pushButton_register_clicked();




    void on_pushButton_login_clicked();

private:
    Ui::login *ui;
    iKernel *m_pkernel;
};

#endif // LOGIN_H
