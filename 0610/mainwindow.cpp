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

    connect(ctx_->connectionService(), &ConnectionService::connectionChanged,
            this, [this](bool connected) {
                ui->statusbar->showMessage(connected ? "已连接" : "未连接");
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

    QString fileName = ui->ledit_data_file_name->text();
    if (fileName.isEmpty()) {
        fileName = "01";
    }

    Result ret = ctx_->trajectoryService()->sendToController(fileName);

    if (!ret.ok) {
        QMessageBox::warning(this, "轨迹下发失败", ret.message);
        return;
    }

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
    qDebug() << "Thread buttons removed — no longer needed.";
}


void MainWindow::on_btn_thread_close_clicked()
{
    qDebug() << "Thread buttons removed — no longer needed.";
}


void MainWindow::on_btn_trace_to_dat_clicked()
{
    trace_generation();
}
