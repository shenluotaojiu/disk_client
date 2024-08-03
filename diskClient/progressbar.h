#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QDialog>

namespace Ui {
class progressbar;
}

class progressbar : public QDialog
{
    Q_OBJECT

public:
    explicit progressbar(QWidget *parent = nullptr);
    ~progressbar();
    void setvalue(int val);

private:
    Ui::progressbar *ui;
};

#endif // PROGRESSBAR_H
