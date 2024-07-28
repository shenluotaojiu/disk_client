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
        qDebug()<<"open false";
    // //-------------------
    // QObject::connect((TCPKernel*)m_pkernel,&TCPKernel::signals_kernel_registerrs
    //                  ,this,&Widget::slots_widget_registerrs);
    // //-------------------

    m_plogin->show();
}

Widget::~Widget()
{
    delete ui;

    delete m_pkernel;
    delete m_plogin;
}

void Widget::slots_widget_registerrs(char result)
{
    switch(result)
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
