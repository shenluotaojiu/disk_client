#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "kernel/tcpkernel.h"
#include <QDebug>
#include "login.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
    iKernel* m_pkernel;
    login *m_plogin;
};
#endif // WIDGET_H
