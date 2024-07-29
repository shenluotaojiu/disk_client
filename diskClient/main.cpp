#include "widget.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;

    // bool bflag = QObject::connect((TCPKernel*)w.m_pkernel,&TCPKernel::signals_kernel_registerrs
    //                  ,&w,&Widget::slots_widget_registerrs,Qt::BlockingQueuedConnection);
    // qDebug()<<bflag;


    w.hide();



    return a.exec();
}
