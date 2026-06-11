#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "./app/AppContext.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

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
    void on_btn_ip_scan_clicked();
    void on_btn_connect_controller_clicked();
    void on_btn_disconnect_controller_clicked();
    void on_btn_trace_test_clicked();
    void on_btn_trace_read_clicked();
    void on_btn_trace_to_xlsx_clicked();
    void on_btn_xlsx_to_dat_clicked();
    void on_btn_thread_open_clicked();
    void on_btn_thread_close_clicked();
    void on_btn_trace_to_dat_clicked();

private:
    Ui::MainWindow *ui;
    AppContext* ctx_;
};
#endif // MAINWINDOW_H
