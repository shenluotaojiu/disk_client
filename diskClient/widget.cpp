#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_pkernel =new TCPKernel();
    m_plogin = new login();
    // 把kernel从widget传到login里
    m_plogin->setKernel(m_pkernel);

    if(!m_pkernel->open())
        QMessageBox::warning(nullptr,"","服务器连接失败");
    //----------信号和槽连接处---------
    QObject::connect((TCPKernel*)m_pkernel,&TCPKernel::signals_kernel_registerrs
                     ,this,&Widget::slots_widget_registerrs,Qt::BlockingQueuedConnection);
    QObject::connect((TCPKernel*)m_pkernel,&TCPKernel::signals_kernel_loginrs
                     ,this,&Widget::slots_widget_loginrs,Qt::BlockingQueuedConnection);
    QObject::connect((TCPKernel*)m_pkernel,&TCPKernel::signals_kernel_getfilelistrs
                     ,this,&Widget::slots_widget_getfilers,Qt::BlockingQueuedConnection);


    //-------------------
    m_plogin->show();
}

Widget::~Widget()
{
    delete ui;

    delete m_pkernel;
    delete m_plogin;
}

void Widget::slots_widget_registerrs(STRU_REGISTER_RS* psrr)
{
    switch(psrr->m_szResult)
    {
    case _register_err:
        QMessageBox::warning(m_plogin,"","注册失败");
        break;
    case _register_success:
        QMessageBox::warning(m_plogin,"","注册成功");
        // 处理
        break;
    }
}

void Widget::slots_widget_loginrs(STRU_LOGIN_RS* pslr)
{
    switch (pslr->m_szResult) {
    case _login_password_err:
        QMessageBox::warning(m_plogin,"","密码错误");
        break;
    case _login_usernoexists:
        QMessageBox::warning(m_plogin,"","用户名不存在");
        break;
    case _login_success:
        m_plogin->close();
        this->show();
        // 发送获取文件列表请求
        STRU_GETFILELIST_RQ sgr;
        sgr.m_userID = pslr->m_userID;
        m_pkernel->sendData((char*)&sgr,sizeof(sgr));
        break;
    }
}

void Widget::slots_widget_getfilers(STRU_GETFILELIST_RS *psgr)
{
    QString str;
    for(int i = 0;i<psgr->m_nFileNum;i++)
    {
        //文件名
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(QIcon(":/file.jpg"),psgr->m_aryFile[i].m_szFileName));

        //文件大小
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(QString::number(psgr->m_aryFile[i].m_llFileSize)));

        //文件上传时间
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(psgr->m_aryFile[i].m_szDateTime));
    }
}
