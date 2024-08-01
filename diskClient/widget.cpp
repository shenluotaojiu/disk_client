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
    QObject::connect((TCPKernel*)m_pkernel,&TCPKernel::signals_kernel_uploadinfors
                     ,this,&Widget::slots_widget_uploadinfors,Qt::BlockingQueuedConnection);


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
        m_userID = pslr->m_userID;
        m_plogin->close();
        this->show();
        // 发送获取文件列表请求
        STRU_GETFILELIST_RQ sgr;
        sgr.m_userID = m_userID;
        m_pkernel->sendData((char*)&sgr,sizeof(sgr));
        break;
    }
}

void Widget::slots_widget_getfilers(STRU_GETFILELIST_RS *psgr)
{
    QString str;
    m_nFileNum = psgr->m_nFileNum;
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

void Widget::slots_widget_uploadinfors(STRU_UPLOADFILEINFO_RS *psur)
{
    stru_filinfo* p = m_mapFileNameToFileInfo[psur->m_szFileName];
    //判断当前结果
    FILE* pfile = fopen(p->m_szPathName,"rb");
    switch(psur->m_szResult)
    {
    case _uploadfile_normal:
    {
        ui->tableWidget->setItem(m_nFileNum,0,new QTableWidgetItem(QIcon(":/file.jpg"),psur->m_szFileName));    // 显示文件名字
        ui->tableWidget->setItem(m_nFileNum,1,new QTableWidgetItem(QString::number(p->m_filesize)));            // 文件大小
        ui->tableWidget->setItem(m_nFileNum,2,new QTableWidgetItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
    }
        break;
    case _uploadfile_continue:
        // 断点续传
        // 移动文件指针到指定位置
        fseek(pfile,psur->m_pos,SEEK_SET);
        break;
    case _uploadfile_flash:
        // 将文件信息显示到控件上，弹出文件上传成功
        {
            ui->tableWidget->setItem(m_nFileNum,0,new QTableWidgetItem(QIcon(":/file.jpg"),psur->m_szFileName));    // 显示文件名字
            ui->tableWidget->setItem(m_nFileNum,1,new QTableWidgetItem(QString::number(p->m_filesize)));            // 文件大小
            ui->tableWidget->setItem(m_nFileNum,2,new QTableWidgetItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));      // 文件时间
        }
        m_nFileNum++;
        QMessageBox::information(this,"上传文件","文件上传成功");
        return;
        break;
    case _uploadfile_isuploaded:
        // 不需要上传了
        QMessageBox::information(this,"上传文件","该文件已经上传过了");
        return;
        break;
    }
    // 读文件并发送
    STRU_UPLOADFILEBLOCK_RQ sur;
    sur.m_fileID = psur->m_fileID;
    sur.m_userID = m_userID;

    while(1)
    {
        size_t nreadnum = fread(sur.m_szFileContent,sizeof(char),sizeof(sur.m_szFileContent),pfile);
        if(nreadnum > 0)
        {
            sur.m_fileblocksize = nreadnum;
            m_pkernel->sendData((char*)&sur,sizeof(sur));
        }
        else
        {
            break;
        }
    }
    fclose(pfile);
}

void Widget::on_pushButton_add_clicked()
{
    //获取文件信息发送服务器（文件名，大小，（时间））
    QString pathName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "/home",
                                                    "Images (*.png *.xpm *.jpg);;Text files (*.txt);;All files (*.*)");

    QString fileName = pathName.section('/',-1);

    QFile file(pathName);
    file.open(QIODevice::ReadOnly);
    long long fileSize = file.size();
    file.close();

    string strMD5 = FileDigest(pathName);

    STRU_UPLOADFILEINFO_RQ sur;
    sur.m_fileSize = fileSize;
    sur.m_userID = m_userID;
    strcpy(sur.m_szFileName,fileName.toStdString().c_str());
    strcpy(sur.m_szMD5,strMD5.c_str());

    m_pkernel->sendData((char*)&sur,sizeof(sur));

    stru_filinfo* p = new stru_filinfo;
    p->m_fileid = 0;
    p->m_filesize = fileSize;
    p->m_pos = 0;
    strcpy(p->m_szPathName,pathName.toStdString().c_str());
    m_mapFileNameToFileInfo[fileName] = p;
}























