#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QMetaType>
#include "zaux.h"
#include "zmotion.h"
#include "robot_trace.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    ZMC_HANDLE g_handle;  // 控制器句柄
    int m_nTimerId;  // what???????
    QThread t_trace_file;
    QThread t_motion_cmd;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 初始化设置
    void Init();

    // 线程管理
    bool thread_open();
    bool thread_close();

    // 控制器IP搜索
    void ip_Scan();

    //
    void up_State();

    // 连接
    void connect_Ether();

    // 断连
    void closeEther();

    // 轨迹输入测试
    void trace_test();
    void trace_generation_test();
    void trace_read_test();
    void dat_to_xlsx();
    void xlsx_to_dat();
    void motion_cmd();  // 从文件读取运动轨迹并下发

    //指令反馈
    void commandCheckHandler(const char *command, int ret);

private slots:
    void on_btn_ip_scan_clicked();

    void on_btn_connect_controller_clicked();

    void on_btn_disconnect_controller_clicked();

    void on_btn_trace_test_clicked();

    void on_btn_trace_generation_test_clicked();

    void on_btn_trace_read_clicked();

    void on_btn_trace_to_xlsx_clicked();

    void on_btn_xlsx_to_dat_clicked();

    void on_btn_thread_open_clicked();

    void on_btn_thread_close_clicked();

signals:
    void s_trace_to_dat_test(const QString& cus_file_name);
    void s_dat_to_xlsx(const QString& dat_file_name, const QString& xlsx_file_name);
    void s_xlsx_to_dat(const QString& dat_file_name, const QString& xlsx_file_name);
    void s_trace_to_controller(ZMC_HANDLE g_handle, const QString& cus_file_name);

public:
    bool thread_flag;

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
