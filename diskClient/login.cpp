#include "login.h"
#include "ui_login.h"

login::login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::login)
{
    ui->setupUi(this);
    setWindowTitle("disk");
    setWindowIcon(QIcon(":/disk.jpg"));
}

login::~login()
{
    delete ui;
}

void login::setKernel(iKernel *m_pkernel)
{
    this->m_pkernel = m_pkernel;
}

void login::on_pushButton_register_clicked()
{
    // 接受ui信息
    QString qstr_tel = ui->lineEdit_register_tel->text();
    QString qstr_ac = ui->lineEdit_register_account->text();
    QString qstr_pw = ui->lineEdit_register_pw->text();
    // 向服务器发送注册请求
    STRU_REGISTER_RQ srr;
    srr.m_tel = qstr_tel.toLongLong();
    strcpy(srr.m_szName,qstr_ac.toStdString().c_str());
    strcpy(srr.m_szPassword,qstr_pw.toStdString().c_str());
    m_pkernel->sendData((char*)&srr,sizeof(srr));

}

