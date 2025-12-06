#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "zaux.h"
#include "zmotion.h"

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

    // 断连
    void closeEther();

private slots:
    void on_btn_ip_scan_clicked();

    void on_btn_connect_controller_clicked();

    void on_btn_disconnect_controller_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
