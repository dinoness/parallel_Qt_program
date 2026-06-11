#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "./app/AppContext.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

enum class MotionMode {
    None,
    DirectJoint,
    CartJog,
    Trace
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(AppContext* context, QWidget *parent = nullptr);
    ~MainWindow();

    void Init();
    void ip_Scan();
    void connect_Ether();
    void closeEther();
    void trace_generation();
    void trace_read_test();
    void dat_to_xlsx();
    void xlsx_to_dat();
    void motion_cmd();

private slots:
    // Connection
    void on_btn_ip_scan_clicked();
    void on_btn_connect_controller_clicked();
    void on_btn_disconnect_controller_clicked();

    // Home
    void on_btn_home_clicked();

    // Direct Joint
    void on_btn_direct_joint_enter_clicked();
    void on_btn_direct_joint_exit_clicked();
    void on_btn_direct_joint_send_clicked();

    // Cart Jog
    void on_btn_jog_enter_clicked();
    void on_btn_jog_exit_clicked();
    void on_btn_jog_send_clicked();

    // Trace
    void on_btn_trace_enter_clicked();
    void on_btn_trace_exit_clicked();
    void on_btn_thread_open_clicked();
    void on_btn_thread_close_clicked();
    void on_btn_trace_send_clicked();

    // Data
    void on_btn_trace_test_clicked();
    void on_btn_trace_read_clicked();
    void on_btn_trace_to_xlsx_clicked();
    void on_btn_xlsx_to_dat_clicked();
    void on_btn_trace_to_dat_clicked();



private:
    // ── 模式切换 ──────────────────────────────────────
    bool canEnterMode(MotionMode mode);
    void enterMode(MotionMode mode);
    void exitCurrentMode();
    void updateMotionModeDisplay();
    void updateTraceButtonStates();

    // ── 运动指令 ──────────────────────────────────────
    void sendDirectJointCmd();
    void sendCartJogCmd();
    void sendHomeCmd();
    void openTraceThread();
    void closeTraceThread();
    void sendTraceFile();

    Ui::MainWindow *ui;
    AppContext* ctx_;

    MotionMode currentMotionMode_ = MotionMode::None;
    bool traceThreadOpened_ = false;
};

#endif // MAINWINDOW_H
