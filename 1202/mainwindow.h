#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "zaux.h"
#include "zmotion.h"
#include "robo_trace.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    ZMC_HANDLE g_handle;  // 控制器句柄
    int m_nTimerId;  // ???????

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 初始化设置
    void Init();

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

    //指令反馈
    void commandCheckHandler(const char *command, int ret);

private slots:
    void on_btn_ip_scan_clicked();

    void on_btn_connect_controller_clicked();

    void on_btn_disconnect_controller_clicked();

    void on_btn_trace_test_clicked();


    void on_btn_trace_generation_test_clicked();

public:
    robo_trace robo_trace_;
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
