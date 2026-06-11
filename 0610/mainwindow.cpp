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

MainWindow::MainWindow(AppContext* context, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , ctx_(context)
{
    ui->setupUi(this);
    setWindowTitle("M");

    Init();
}

MainWindow::~MainWindow()
{
    thread_close();
    delete ui;
}


void MainWindow::Init()
{
    if (ctx_ == nullptr) {
        QMessageBox::critical(this, "错误", "AppContext 未初始化");
        return;
    }

    connect(ctx_->connectionService(), &ConnectionService::connectionChanged,
            this, [this](bool connected) {
                ui->statusbar->showMessage(connected ? "已连接" : "未连接");
            });

    // g_handle = 0;
    // m_nTimerId = -1;
    // thread_flag = false;

    // qRegisterMetaType<ZMC_HANDLE>("ZMC_HANDLE");

    // thread_open();

    // ui->ledit_data_file_name->setText("02");
    // ui->ledit_xlsx_file_name->setText("11");


}


bool MainWindow::thread_open()
{
    if(thread_flag == true)
    {
        qDebug() << "Threads have opend.";
        return false;
    }

    // File thread
    robot_trace *p_trace_file = new robot_trace;
    p_trace_file->moveToThread(&t_trace_file);

    // structure (发出对象，发出类中的信号，接收对象，接收对象类中的槽)
    // 对于Cannot queue arguments of type，需要引入头文件#include <QMetaType>,然后在构造函数中定义对应的类型
    connect(this, &MainWindow::s_trace_to_dat_test, p_trace_file, &robot_trace::trace_to_dat_test);
    connect(this, &MainWindow::s_dat_to_xlsx, p_trace_file, &robot_trace::dat_to_xlsx);
    connect(this, &MainWindow::s_xlsx_to_dat, p_trace_file, &robot_trace::xlsx_to_dat);


    t_trace_file.start();

    // Motion command thread
    robot_trace *p_motion_cmd = new robot_trace;
    p_motion_cmd->moveToThread(&t_motion_cmd);

    connect(this, &MainWindow::s_trace_to_controller, p_motion_cmd, &robot_trace::trace_to_controller);

    t_motion_cmd.start();



    qDebug() << "Threads opend success.";

    thread_flag = true;
    return true;
}


bool MainWindow::thread_close()
{
    if(thread_flag == false)
    {
        qDebug() << "Threads are ont open yet.";
        return false;
    }
    t_trace_file.quit();
    t_trace_file.wait();

    t_motion_cmd.quit();
    t_motion_cmd.wait();

    qDebug() << "Threads close success.";
    thread_flag = false;
    return true;
}


void MainWindow::ip_Scan()
{
    if (ctx_ == nullptr) {
        return;
    }

    ui->comboBox->clear();
    ui->comboBox->addItem("127.0.0.1");

    QStringList ipList;
    Result ret = ctx_->connectionService()->ipScan(ipList);

    if (!ret.ok) {
        qDebug() << "IP scan failed:" << ret.message;
        return;
    }

    ui->comboBox->addItems(ipList);
    qDebug() << "IP scan found:" << ipList;
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
    if (ctx_ == nullptr) {
        QMessageBox::warning(this, "错误", "AppContext 为空");
        return;
    }

    QString ip = ui->comboBox->currentText();

    if (ip.isEmpty()) {
        QMessageBox::warning(this, "错误", "请输入控制器 IP");
        return;
    }

    Result ret = ctx_->connectionService()->connectToController(ip);

    if (!ret.ok) {
        QMessageBox::warning(this, "连接失败", ret.message);
        return;
    }

    QMessageBox::information(this, "连接成功", "控制器连接成功");
}



void MainWindow::closeEther()
{
    if (ctx_ == nullptr) {
        return;
    }

    Result ret = ctx_->connectionService()->disconnectFromController();

    if (!ret.ok) {
        QMessageBox::warning(this, "断开失败", ret.message);
        return;
    }

    QMessageBox::information(this, "已断开", "控制器连接已断开");
}



void MainWindow::trace_test()
{
    int ret;
    // 轨迹生成可以是等时间变距离，也可以是等距离变时间，或者两者都变
    uint32 Tims[DataGroupSize];  // PT运动时间规划  这样可以变间隔
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

void MainWindow::trace_generation_test()
{
    if(thread_flag == false)
    {
        qDebug() << "Threads are ont open yet.";
        return;
    }

    emit s_trace_to_dat_test(ui->ledit_data_file_name->text());
    qDebug() << "Trace generation over.";
    qDebug() << "File is in trace_data folder.";
}

void MainWindow::trace_read_test()
{
    qDebug() << "The button is blank.";
}

void MainWindow::dat_to_xlsx()
{
    if(thread_flag == false)
    {
        qDebug() << "Threads are ont open yet.";
        return;
    }

    emit s_dat_to_xlsx(ui->ledit_data_file_name->text(), ui->ledit_xlsx_file_name->text());
    qDebug() << "dat_to_xlsx over";
}

void MainWindow::xlsx_to_dat()
{
    if(thread_flag == false)
    {
        qDebug() << "Threads are ont open yet.";
        return;
    }

    emit s_xlsx_to_dat(ui->ledit_data_file_name->text(), ui->ledit_xlsx_file_name->text());
    qDebug() << "xlsx_to_dat over";
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

void MainWindow::motion_cmd()
{
    emit s_trace_to_controller(g_handle, ui->ledit_data_file_name->text());
    qDebug() << "Motion command over.";
}

// ======================= SLOT FUNCTION ==========================

void MainWindow::on_btn_ip_scan_clicked()
{
    ip_Scan();
}

void MainWindow::on_btn_connect_controller_clicked()
{
    connect_Ether();
}


void MainWindow::on_btn_disconnect_controller_clicked()
{
    closeEther();
}


void MainWindow::on_btn_trace_test_clicked()
{
    // trace_test();
    motion_cmd();
}


void MainWindow::on_btn_trace_read_clicked()
{
    trace_read_test();
}


void MainWindow::on_btn_trace_to_xlsx_clicked()
{
    dat_to_xlsx();
}


void MainWindow::on_btn_xlsx_to_dat_clicked()
{
    xlsx_to_dat();
}


void MainWindow::on_btn_thread_open_clicked()
{
    thread_open();
}


void MainWindow::on_btn_thread_close_clicked()
{
    thread_close();
}


void MainWindow::on_btn_trace_to_dat_clicked()
{
    trace_generation_test();
}

