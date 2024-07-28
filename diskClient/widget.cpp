#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    m_pkernel =new TCPKernel();
    m_plogin = new login();

    if(!m_pkernel->open())
        qDebug()<<"open false";

    m_plogin->show();
}

Widget::~Widget()
{
    delete ui;

    delete m_pkernel;
    delete m_plogin;
}
