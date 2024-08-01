#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "kernel/tcpkernel.h"
#include <QDebug>
#include "login.h"
#include <QObject>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include "md5.h"
#include <map>
#include <QDateTime>
#include <QTime>

struct  stru_filinfo
{
    char m_szPathName[MAX_PATH];
    long long m_filesize;
    long long m_fileid;
    long long m_pos;
};


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
    string FileDigest(QString filename) {

        QFile file(filename);

        file.open(QIODevice::ReadOnly);
        if (!file.isOpen())
            return "";

        MD5 md5;
        char buffer[1024];
        while (1)
        {
            qint64 length =  file.read(buffer, 1024);

            if (length > 0)
                md5.update(buffer, length);
            else
                break;
        }
        file.close();
        return md5.toString();
    }

public slots:
    void slots_widget_registerrs(STRU_REGISTER_RS* psrr);
    void slots_widget_loginrs(STRU_LOGIN_RS* pslr);
    void slots_widget_getfilers(STRU_GETFILELIST_RS* psgr);
    void slots_widget_uploadinfors(STRU_UPLOADFILEINFO_RS* psur);

private:
    Ui::Widget *ui;
    long long m_userID;
    iKernel* m_pkernel;
    login *m_plogin;
    std::map<QString,stru_filinfo*> m_mapFileNameToFileInfo;
    int m_nFileNum = 0;


private slots:
    void on_pushButton_add_clicked();
};
#endif // WIDGET_H
