#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QString>

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
    delete ui;
}


void MainWindow::Init()
{
    if (ctx_ == nullptr) {
        QMessageBox::critical(this, "错误", "AppContext 未初始化");
        return;
    }

    // 连接状态
    connect(ctx_->connectionService(), &ConnectionService::connectionChanged,
            this, [this](bool connected) {
                ui->statusbar->showMessage(connected ? "已连接" : "未连接");
            });

    // 轨迹下发进度
    connect(ctx_->trajectoryService(), &TrajectoryService::sendProgressChanged,
            this, [this](int sentGroups, int totalGroups, int percent) {
                ui->statusbar->showMessage(
                    QString("轨迹下发中... %1% (%2/%3 组)")
                        .arg(percent).arg(sentGroups).arg(totalGroups));
            });

    // 轨迹下发完成
    connect(ctx_->trajectoryService(), &TrajectoryService::sendFinished,
            this, [this](const Result& result) {
                if (result.ok) {
                    ui->statusbar->showMessage("轨迹下发完成", 5000);
                    QMessageBox::information(this, "成功", "轨迹下发完成");
                } else {
                    ui->statusbar->showMessage("轨迹下发失败", 5000);
                    QMessageBox::warning(this, "轨迹下发失败", result.message);
                }
            });

    // 下发状态切换（控制按钮）
    connect(ctx_->trajectoryService(), &TrajectoryService::sendingStateChanged,
            this, [this](bool sending) {
                ui->btn_trace_send->setEnabled(!sending);
                ui->btn_thread_open->setEnabled(sending);
            });
}


void MainWindow::ip_Scan()
{
    if (ctx_ == nullptr) return;

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
    if (ctx_ == nullptr) return;

    Result ret = ctx_->connectionService()->disconnectFromController();

    if (!ret.ok) {
        QMessageBox::warning(this, "断开失败", ret.message);
        return;
    }

    QMessageBox::information(this, "已断开", "控制器连接已断开");
}


void MainWindow::trace_generation()
{
    if (ctx_ == nullptr) return;

    QString fileName = ui->ledit_data_file_name->text();
    if (fileName.isEmpty()) {
        fileName = "01";
    }

    Result ret = ctx_->trajectoryService()->generateAndSave(fileName);

    if (!ret.ok) {
        QMessageBox::warning(this, "生成失败", ret.message);
        return;
    }

    qDebug() << "Trace generation over. File is in trace_data folder.";
}

void MainWindow::trace_read_test()
{
    qDebug() << "The button is blank.";
}

void MainWindow::dat_to_xlsx()
{
    if (ctx_ == nullptr) return;

    QString datFile = ui->ledit_data_file_name->text();
    QString csvFile = ui->ledit_xlsx_file_name->text();
    if (datFile.isEmpty()) datFile = "01";
    if (csvFile.isEmpty()) csvFile = datFile;

    Result ret = ctx_->trajectoryService()->datToCsv(datFile, csvFile);

    if (!ret.ok) {
        QMessageBox::warning(this, "导出失败", ret.message);
        return;
    }

    qDebug() << "dat_to_csv over";
}

void MainWindow::xlsx_to_dat()
{
    if (ctx_ == nullptr) return;

    QString csvFile = ui->ledit_xlsx_file_name->text();
    QString datFile = ui->ledit_data_file_name->text();
    if (csvFile.isEmpty()) csvFile = "01";
    if (datFile.isEmpty()) datFile = csvFile;

    Result ret = ctx_->trajectoryService()->csvToDat(csvFile, datFile);

    if (!ret.ok) {
        QMessageBox::warning(this, "导入失败", ret.message);
        return;
    }

    qDebug() << "csv_to_dat over";
}

void MainWindow::motion_cmd()
{
    if (ctx_ == nullptr) return;

    if (ctx_->trajectoryService()->isSending()) {
        QMessageBox::warning(this, "提示", "当前已有轨迹正在下发");
        return;
    }

    if (!ctx_->driver()->isOpen()) {
        QMessageBox::warning(this, "错误", "控制器未连接");
        return;
    }

    QString fileName = ui->ledit_data_file_name->text();
    if (fileName.isEmpty()) {
        fileName = "01";
    }

    // 异步下发——Service 内部负责读文件 + 启动 worker 线程
    Result ret = ctx_->trajectoryService()->startSendTrajectoryAsync(fileName);

    if (!ret.ok) {
        QMessageBox::warning(this, "轨迹下发启动失败", ret.message);
        return;
    }
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
    // 取消轨迹下发
    ctx_->trajectoryService()->cancelSendTrajectory();
    ui->btn_thread_open->setEnabled(false);
    ui->statusbar->showMessage("正在取消轨迹下发...");
}


void MainWindow::on_btn_thread_close_clicked()
{
    qDebug() << "btn_thread_close clicked.";
}


void MainWindow::on_btn_trace_to_dat_clicked()
{
    trace_generation();
}

void MainWindow::on_btn_direct_joint_enter_clicked()
{

}


void MainWindow::on_btn_direct_joint_exit_clicked()
{

}


void MainWindow::on_btn_direct_joint_send_clicked()
{

}


void MainWindow::on_btn_home_clicked()
{

}


void MainWindow::on_btn_jog_enter_clicked()
{

}


void MainWindow::on_btn_jog_exit_clicked()
{

}


void MainWindow::on_btn_jog_send_clicked()
{

}


void MainWindow::on_btn_trace_enter_clicked()
{

}


void MainWindow::on_btn_jog_exit_2_clicked()
{

}


void MainWindow::on_btn_trace_send_clicked()
{

}

