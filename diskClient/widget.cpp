#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_pkernel =new TCPKernel();
    m_plogin = new login();
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    m_tp.createThreadPool(1,si.dwNumberOfProcessors*2);
    // 把kernel从widget传到login里
    m_plogin->setKernel(m_pkernel);

    //--进度条--

    //------

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
    QObject::connect((TCPKernel*)m_pkernel,&TCPKernel::signals_kernel_downloadfilers
                     ,this,&Widget::slots_widget_downloadfilers,Qt::BlockingQueuedConnection);
    QObject::connect((TCPKernel*)m_pkernel,&TCPKernel::signals_kernel_downloadblockrs
                     ,this,&Widget::slots_widget_downloadblockrs,Qt::BlockingQueuedConnection);


    //-------------------
    m_plogin->show();
}

Widget::~Widget()
{
    delete ui;

    delete m_pkernel;
    delete m_plogin;
}

bool Widget::setDownloadFilePath()
{
    QString pathName = QFileDialog::getExistingDirectory(this, tr("Open File"),
                                                    "/home",QFileDialog::ShowDirsOnly);
    if(pathName == "")
    {
        // 有问题，不让下载
        return false;
    }

    m_szDownloadPath = pathName;
    return true;
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
    for(int i = 0;i<psgr->m_nFileNum;i++)
    {
        ui->tableWidget->setRowCount(m_nFileNum+1);
        //文件名
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(QIcon(":/file.jpg"),psgr->m_aryFile[i].m_szFileName));

        //文件大小
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(QString::number(psgr->m_aryFile[i].m_llFileSize)));

        //文件上传时间
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(psgr->m_aryFile[i].m_szDateTime));
        m_nFileNum++;
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
        ui->tableWidget->setRowCount(m_nFileNum+1);
        ui->tableWidget->setItem(m_nFileNum,0,new QTableWidgetItem(QIcon(":/file.jpg"),psur->m_szFileName));    // 显示文件名字
        ui->tableWidget->setItem(m_nFileNum,1,new QTableWidgetItem(QString::number(p->m_filesize)));            // 文件大小
        ui->tableWidget->setItem(m_nFileNum,2,new QTableWidgetItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
        m_nFileNum++;
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
            ui->tableWidget->setRowCount(m_nFileNum+1);
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
    // 文件id与文件pos
    p->m_fileid = psur->m_fileID;
    p->m_pos = psur->m_pos;
    itask *ptask = new sendFile(m_pkernel,m_userID,p,pfile);
    m_tp.push(ptask);

}

void Widget::slots_widget_downloadfilers(STRU_DOWNLOADFILE_RS *psdr)
{
    //获取文件地址
    char szPath[MAX_PATH] = {0};
    sprintf(szPath,"%s/%s",m_szDownloadPath.toStdString().c_str(),psdr->fileName);
    //匹对下载地址内是否有文件
    QFile qfile(szPath);
    STRU_DOWNLOADBLOCK_RQ sdr;
    sdr.fileId = psdr->f_id;

    FILE* m_pfile = fopen(szPath,"wb");
    //有->断点续传
    if(qfile.exists())
    {
        qfile.open(QIODevice::ReadOnly);
        //偏移量为当前文件size；
        sdr.m_pos = qfile.size();
        sdr.m_szResult = _downloadfile_continue;

    }
    else//无->正常传
    {
        sdr.m_pos = 0;
        sdr.m_szResult = _downloadfile_normal;
    }
    if(m_mapFileIdToFileInfo.find(sdr.fileId) == m_mapFileIdToFileInfo.end())
    {
        stru_filinfo* p = new stru_filinfo();
        p->m_fileid = psdr->f_id;
        p->m_filesize = sdr.fileSize;
        p->m_pos = sdr.m_pos;
        p->m_pfile = m_pfile;
        m_mapFileIdToFileInfo[sdr.fileId] = p;
    }


    //发送请求
    m_pkernel->sendData((char*)&sdr,sizeof(sdr));
}

void Widget::slots_widget_downloadblockrs(STRU_DOWNLOADBLOCK_RS *psdr)
{
    stru_filinfo* p = m_mapFileIdToFileInfo[psdr->m_fileID];
    //将文件内容写入文件，等到接收大小总和等于文件大小时停止接收
    size_t nWriteNum = fwrite(psdr->m_szFileContent,sizeof(char),psdr->m_fileblocksize,p->m_pfile);
    if(nWriteNum > 0)
        p->m_pos+=nWriteNum;

    if(p->m_pos == psdr->m_fileSize)
    {
        fclose(p->m_pfile);
        delete p;p = nullptr;
        m_mapFileIdToFileInfo.erase(psdr->m_fileID);
    }

}

void Widget::on_pushButton_add_clicked()
{
    //获取文件信息发送服务器（文件名，大小，（时间））
    QString pathName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "/home",
                                                    "Images (*.png *.xpm *.jpg);;Text files (*.txt);;All files (*.*)");

    if(pathName == "")
        return;

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

void sendFile::run()
{
    STRU_UPLOADFILEBLOCK_RQ sur;
    sur.m_fileID = m_pfileinfo->m_fileid;
    sur.m_userID = m_userId;

    while(1)
    {
        size_t nreadnum = fread(sur.m_szFileContent,sizeof(char),sizeof(sur.m_szFileContent),m_pfile);
        if(nreadnum > 0)
        {
            sur.m_fileblocksize = nreadnum;
            m_pKernel->sendData((char*)&sur,sizeof(sur));
        }
        else
        {
            break;
        }
    }
    fclose(m_pfile);
}

void Widget::on_pushButton_share_clicked()
{
    //获取分享文件名
    int nRow = ui->tableWidget->currentRow();
    if(nRow == -1)
        return;
    QString fileName = ui->tableWidget->item(nRow,0)->text();

    //生成提取码
    QString str;
    srand(time(NULL));
    for(int i=0;i<4;i++)
        str+=rand()%26 + 'a';

    //发送服务器
    STRU_SHARELINK_RQ ssr;
    strcpy(ssr.m_szCode,str.toStdString().c_str());
    strcpy(ssr.m_szFileName,fileName.toStdString().c_str());
    ssr.m_userID = m_userID;

    m_pkernel->sendData((char*)&ssr,sizeof(ssr));
}


void Widget::on_pushButton_download_clicked()
{
    // 获得下载路径
    if(setDownloadFilePath() == false)
    {
        QMessageBox::information(this,"下载失败","请选择正确的下载路径");
        return;
    }
    // 获得文件信息，文件名
    int nRow = ui->tableWidget->currentRow();
    if(nRow == -1)
        return;
    QString fileName = ui->tableWidget->item(nRow,0)->text();
    // 发送到服务器
    STRU_DOWNLOADFILE_RQ sdr;
    sdr.userId = m_userID;
    strcpy(sdr.fileName,fileName.toStdString().c_str());
    m_pkernel->sendData((char*)&sdr,sizeof(sdr));
}

