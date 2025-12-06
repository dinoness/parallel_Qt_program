#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QCloseEvent>
#include <QString>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("M");
    Init();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::Init()
{
    g_handle = 0;
    m_nTimerId = -1;
}

void MainWindow::ip_Scan()
{
    ui->comboBox->clear();
    ui->comboBox->addItem("127.0.0.1");
    char * ip = new char[10240];
    ZAux_SearchEthlist(ip,10230,200);
    QString ips(ip);
    QStringList iplist = ips.split(' ');
    qDebug() << iplist;
    ui->comboBox->addItems(iplist);
}


void MainWindow::up_State()
{
    if(0 == g_handle)
    {
        // setWindowTitle("PC与下位机程序通讯");
        return ;
    }
    // float Dpos = 0;
    // QString pos;

    // ZAux_Direct_GetDpos(g_handle,0,&Dpos);
    // pos = QString().number(Dpos);
    // ui->edit_State_X->setText(pos);

    // ZAux_Direct_GetDpos(g_handle,1,&Dpos);
    // pos = QString().number(Dpos);
    // ui->edit_State_Y->setText(pos);

    // ZAux_Direct_GetDpos(g_handle,2,&Dpos);
    // pos = QString().number(Dpos);
    // ui->edit_State_Z->setText(pos);

}


void MainWindow::closeEther()
{
    if(0 != g_handle)
    {
        ZAux_Close(g_handle);
        g_handle = nullptr;
        killTimer(m_nTimerId);
        m_nTimerId = -1;
        qDebug() << "关闭";
    }
}


// =========== SLOT FUNCTION ===========

// ip 扫描按钮
void MainWindow::on_btn_ip_scan_clicked()
{
    ip_Scan();
}

// 控制器连接按钮
void MainWindow::on_btn_connect_controller_clicked()
{
    int32 iresult;
    char * tmp_buff = new char[16];
    QString str;
    QString str_title;

    str = ui->comboBox->currentText();
    qDebug()<<"当前的ip是 ："<<str;
    QByteArray ba = str.toLatin1();
    tmp_buff = ba.data();
    qDebug()<<"current ip tmp_buff"<<tmp_buff;

    if(0 != g_handle)
    {
        ZAux_Close(g_handle);  // 与控制器断开连接
        killTimer(m_nTimerId);
        // setWindowTitle("PC与下位机程序通讯");
    }

    iresult = ZAux_OpenEth(tmp_buff,&g_handle);
    if( 0 != iresult)
    {
        QMessageBox::warning(this,"提示","连接失败");
    }
    else
    {
        QMessageBox::warning(this,"提示","连接成功");
        str_title = windowTitle() + tmp_buff;
        setWindowTitle(str_title);
        m_nTimerId = startTimer(10);  // 每隔10ms触发一次定时器事件，以用这个 ID 来管理和停止定时器
        up_State();
    }
}


void MainWindow::on_btn_disconnect_controller_clicked()
{
    closeEther();
}














