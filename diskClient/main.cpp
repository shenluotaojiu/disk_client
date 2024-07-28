#include "widget.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;

    QObject::connect((TCPKernel*)w.m_pkernel,&TCPKernel::signals_kernel_registerrs
                     ,&w,&Widget::slots_widget_registerrs,Qt::QueuedConnection);


    w.hide();



    return a.exec();
}
