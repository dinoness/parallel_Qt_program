#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QCloseEvent>
#include <QString>
#include <QFileDialog>
#include <math.h>
#include <windows.h>

#define PI 3.14159
#define F_DataUpdate (uint16)1
#define F_DataUsed (uint16)2
#define F_DataBlank (uint16)3
#define DataStart_Index 1000
#define DataGroupSize 100

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

void MainWindow::connect_Ether()
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

void MainWindow::trace_test()
{
    int ret;
    // 轨迹生成（需要考虑轨迹的存储与读取）
    uint32 Tims[DataGroupSize];  // PT运动时间规划
    float DposList[DataGroupSize];  // PT运动距离规划
    int i;
    for(i = 0; i < DataGroupSize; i++)
    {
        Tims[i] = DataGroupSize;
        DposList[i] = (sin(((2*PI)/DataGroupSize)*i))*180;
    }

    ZAux_Trigger(g_handle);

    // 数据传递思路
    // 将数据的本质是一系列离散的距离，在控制器中通过MOVE_PT系列执行
    // 控制器有指令缓冲机制，一旦缓冲区满了，程序就会卡住
    // 等待缓冲区有空间后再运行程序将指令写入缓冲区
    // 因此只要一直发数据，不要空就行
    // 此处指令下发打算通过TABLE传递，存储到几个TABLE块中，通过TABLE块作为缓冲机制
    // 在MODBUS的REG中写入TABLE块的状态值，分为空3，已更新1，已使用2
    // 要调整的点：TABLE块的大小和数量，以及分区的约定
    for(i = 0; i < 3; i++)
    {
        uint16 data_state;
        data_state = F_DataBlank;
        do
        {
            ret = ZAux_Modbus_Get4x(g_handle, i, 1, &data_state);
            commandCheckHandler("ZAux_Modbus_Get4x", ret);

        }while(!(data_state != F_DataUpdate));


        int table_index = DataStart_Index + i * DataGroupSize;
        ZAux_Direct_SetTable(g_handle, table_index, DataGroupSize, DposList);  // 句柄，起始编号，写入数量，数据值
        //35*50ms,走一个周期 y=sin(x)的正弦曲线
        data_state = F_DataUpdate;
        ZAux_Modbus_Set4x(g_handle, i, 1, &data_state);

        Sleep(10);//隔 34*50ms 发一次
    }

    Sleep(100);
    qDebug() << "Trace test over.";
}


void MainWindow::commandCheckHandler(const char *command, int ret)
{
    if (ret)//非 0 则失败
    {
        qDebug() << command << "fail!return code is" << ret << "\n";
        Sleep(2000);
        exit(0);
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
    /*
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
*/
    connect_Ether();
}


void MainWindow::on_btn_disconnect_controller_clicked()
{
    closeEther();
}




void MainWindow::on_btn_trace_test_clicked()
{
    trace_test();
}

