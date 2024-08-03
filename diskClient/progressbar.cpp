#include "progressbar.h"
#include "ui_progressbar.h"

progressbar::progressbar(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::progressbar)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
}

progressbar::~progressbar()
{
    delete ui;
}

void progressbar::setvalue(int val)
{
    ui->progressBar->setValue(val);
}
