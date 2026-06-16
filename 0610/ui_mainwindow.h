/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QComboBox *comboBox;
    QPushButton *btn_ip_scan;
    QPushButton *btn_connect_controller;
    QPushButton *btn_disconnect_controller;
    QPushButton *btn_trace_send;
    QLineEdit *ledit_data_file_name;
    QLabel *label;
    QPushButton *btn_trace_to_dat;
    QPushButton *btn_trace_read;
    QLineEdit *ledit_csv_file_name;
    QLabel *label_2;
    QPushButton *btn_trace_to_xlsx;
    QPushButton *btn_xlsx_to_dat;
    QPushButton *btn_thread_open;
    QPushButton *btn_thread_close;
    QLabel *label_3;
    QLabel *label_4;
    QPushButton *btn_home;
    QLabel *label_5;
    QLabel *label_system_state;
    QLabel *label_6;
    QPushButton *btn_direct_joint_enter;
    QLineEdit *ledit_speed_level;
    QLabel *label_7;
    QLineEdit *ledit_d_j1;
    QLineEdit *ledit_d_j2;
    QLineEdit *ledit_d_j3;
    QLineEdit *ledit_d_j4;
    QLineEdit *ledit_d_j5;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *label_10;
    QLabel *label_11;
    QLabel *label_12;
    QPushButton *btn_direct_joint_exit;
    QPushButton *btn_direct_joint_send;
    QLabel *label_13;
    QPushButton *btn_jog_enter;
    QPushButton *btn_jog_exit;
    QPushButton *btn_jog_send;
    QLabel *label_14;
    QLabel *label_15;
    QLabel *label_16;
    QLabel *label_17;
    QLabel *label_18;
    QLineEdit *ledit_jog_x;
    QLineEdit *ledit_jog_y;
    QLineEdit *ledit_jog_z;
    QLineEdit *ledit_jog_phi;
    QLineEdit *ledit_jog_theta;
    QLabel *label_19;
    QPushButton *btn_trace_enter;
    QPushButton *btn_trace_exit;
    QLabel *label_20;
    QLabel *label_motion_mode;
    QLineEdit *ledit_cmd_id;
    QLabel *label_21;
    QPushButton *btn_pause_resume;
    QPushButton *btn_stop;
    QPushButton *btn_emergency_stop;
    QPushButton *btn_error_reset;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1027, 494);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        comboBox = new QComboBox(centralwidget);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setGeometry(QRect(52, 50, 141, 25));
        btn_ip_scan = new QPushButton(centralwidget);
        btn_ip_scan->setObjectName(QString::fromUtf8("btn_ip_scan"));
        btn_ip_scan->setGeometry(QRect(210, 50, 93, 28));
        btn_connect_controller = new QPushButton(centralwidget);
        btn_connect_controller->setObjectName(QString::fromUtf8("btn_connect_controller"));
        btn_connect_controller->setGeometry(QRect(320, 50, 111, 28));
        btn_disconnect_controller = new QPushButton(centralwidget);
        btn_disconnect_controller->setObjectName(QString::fromUtf8("btn_disconnect_controller"));
        btn_disconnect_controller->setGeometry(QRect(450, 50, 111, 28));
        btn_trace_send = new QPushButton(centralwidget);
        btn_trace_send->setObjectName(QString::fromUtf8("btn_trace_send"));
        btn_trace_send->setGeometry(QRect(610, 300, 111, 28));
        ledit_data_file_name = new QLineEdit(centralwidget);
        ledit_data_file_name->setObjectName(QString::fromUtf8("ledit_data_file_name"));
        ledit_data_file_name->setGeometry(QRect(632, 350, 81, 25));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(548, 350, 81, 20));
        btn_trace_to_dat = new QPushButton(centralwidget);
        btn_trace_to_dat->setObjectName(QString::fromUtf8("btn_trace_to_dat"));
        btn_trace_to_dat->setGeometry(QRect(730, 350, 111, 28));
        btn_trace_read = new QPushButton(centralwidget);
        btn_trace_read->setObjectName(QString::fromUtf8("btn_trace_read"));
        btn_trace_read->setGeometry(QRect(730, 390, 111, 28));
        ledit_csv_file_name = new QLineEdit(centralwidget);
        ledit_csv_file_name->setObjectName(QString::fromUtf8("ledit_csv_file_name"));
        ledit_csv_file_name->setGeometry(QRect(632, 390, 81, 25));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(550, 390, 81, 19));
        btn_trace_to_xlsx = new QPushButton(centralwidget);
        btn_trace_to_xlsx->setObjectName(QString::fromUtf8("btn_trace_to_xlsx"));
        btn_trace_to_xlsx->setGeometry(QRect(860, 350, 111, 28));
        btn_xlsx_to_dat = new QPushButton(centralwidget);
        btn_xlsx_to_dat->setObjectName(QString::fromUtf8("btn_xlsx_to_dat"));
        btn_xlsx_to_dat->setGeometry(QRect(860, 390, 111, 28));
        btn_thread_open = new QPushButton(centralwidget);
        btn_thread_open->setObjectName(QString::fromUtf8("btn_thread_open"));
        btn_thread_open->setGeometry(QRect(800, 260, 161, 28));
        btn_thread_close = new QPushButton(centralwidget);
        btn_thread_close->setObjectName(QString::fromUtf8("btn_thread_close"));
        btn_thread_close->setGeometry(QRect(800, 300, 161, 28));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(50, 20, 201, 19));
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(50, 100, 69, 19));
        btn_home = new QPushButton(centralwidget);
        btn_home->setObjectName(QString::fromUtf8("btn_home"));
        btn_home->setGeometry(QRect(50, 130, 141, 28));
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(600, 30, 101, 21));
        label_system_state = new QLabel(centralwidget);
        label_system_state->setObjectName(QString::fromUtf8("label_system_state"));
        label_system_state->setGeometry(QRect(720, 30, 281, 19));
        label_6 = new QLabel(centralwidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(50, 190, 111, 31));
        btn_direct_joint_enter = new QPushButton(centralwidget);
        btn_direct_joint_enter->setObjectName(QString::fromUtf8("btn_direct_joint_enter"));
        btn_direct_joint_enter->setGeometry(QRect(170, 190, 111, 28));
        ledit_speed_level = new QLineEdit(centralwidget);
        ledit_speed_level->setObjectName(QString::fromUtf8("ledit_speed_level"));
        ledit_speed_level->setGeometry(QRect(230, 130, 91, 25));
        label_7 = new QLabel(centralwidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(230, 100, 101, 19));
        ledit_d_j1 = new QLineEdit(centralwidget);
        ledit_d_j1->setObjectName(QString::fromUtf8("ledit_d_j1"));
        ledit_d_j1->setGeometry(QRect(90, 240, 61, 25));
        ledit_d_j2 = new QLineEdit(centralwidget);
        ledit_d_j2->setObjectName(QString::fromUtf8("ledit_d_j2"));
        ledit_d_j2->setGeometry(QRect(90, 280, 61, 25));
        ledit_d_j3 = new QLineEdit(centralwidget);
        ledit_d_j3->setObjectName(QString::fromUtf8("ledit_d_j3"));
        ledit_d_j3->setGeometry(QRect(90, 320, 61, 25));
        ledit_d_j4 = new QLineEdit(centralwidget);
        ledit_d_j4->setObjectName(QString::fromUtf8("ledit_d_j4"));
        ledit_d_j4->setGeometry(QRect(90, 360, 61, 25));
        ledit_d_j5 = new QLineEdit(centralwidget);
        ledit_d_j5->setObjectName(QString::fromUtf8("ledit_d_j5"));
        ledit_d_j5->setGeometry(QRect(90, 400, 61, 25));
        label_8 = new QLabel(centralwidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(50, 240, 31, 21));
        label_9 = new QLabel(centralwidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(50, 280, 31, 21));
        label_10 = new QLabel(centralwidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(50, 320, 21, 21));
        label_11 = new QLabel(centralwidget);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(50, 360, 31, 21));
        label_12 = new QLabel(centralwidget);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(50, 400, 21, 21));
        btn_direct_joint_exit = new QPushButton(centralwidget);
        btn_direct_joint_exit->setObjectName(QString::fromUtf8("btn_direct_joint_exit"));
        btn_direct_joint_exit->setGeometry(QRect(170, 230, 111, 28));
        btn_direct_joint_send = new QPushButton(centralwidget);
        btn_direct_joint_send->setObjectName(QString::fromUtf8("btn_direct_joint_send"));
        btn_direct_joint_send->setGeometry(QRect(170, 270, 111, 28));
        label_13 = new QLabel(centralwidget);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setGeometry(QRect(320, 190, 69, 31));
        btn_jog_enter = new QPushButton(centralwidget);
        btn_jog_enter->setObjectName(QString::fromUtf8("btn_jog_enter"));
        btn_jog_enter->setGeometry(QRect(400, 190, 111, 28));
        btn_jog_exit = new QPushButton(centralwidget);
        btn_jog_exit->setObjectName(QString::fromUtf8("btn_jog_exit"));
        btn_jog_exit->setGeometry(QRect(400, 230, 111, 28));
        btn_jog_send = new QPushButton(centralwidget);
        btn_jog_send->setObjectName(QString::fromUtf8("btn_jog_send"));
        btn_jog_send->setGeometry(QRect(400, 270, 111, 28));
        label_14 = new QLabel(centralwidget);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setGeometry(QRect(310, 240, 31, 19));
        label_15 = new QLabel(centralwidget);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setGeometry(QRect(310, 280, 21, 19));
        label_16 = new QLabel(centralwidget);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        label_16->setGeometry(QRect(310, 320, 31, 19));
        label_17 = new QLabel(centralwidget);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setGeometry(QRect(310, 360, 21, 19));
        label_18 = new QLabel(centralwidget);
        label_18->setObjectName(QString::fromUtf8("label_18"));
        label_18->setGeometry(QRect(310, 400, 21, 19));
        ledit_jog_x = new QLineEdit(centralwidget);
        ledit_jog_x->setObjectName(QString::fromUtf8("ledit_jog_x"));
        ledit_jog_x->setGeometry(QRect(330, 240, 61, 25));
        ledit_jog_y = new QLineEdit(centralwidget);
        ledit_jog_y->setObjectName(QString::fromUtf8("ledit_jog_y"));
        ledit_jog_y->setGeometry(QRect(330, 280, 61, 25));
        ledit_jog_z = new QLineEdit(centralwidget);
        ledit_jog_z->setObjectName(QString::fromUtf8("ledit_jog_z"));
        ledit_jog_z->setGeometry(QRect(330, 320, 61, 25));
        ledit_jog_phi = new QLineEdit(centralwidget);
        ledit_jog_phi->setObjectName(QString::fromUtf8("ledit_jog_phi"));
        ledit_jog_phi->setGeometry(QRect(330, 360, 61, 25));
        ledit_jog_theta = new QLineEdit(centralwidget);
        ledit_jog_theta->setObjectName(QString::fromUtf8("ledit_jog_theta"));
        ledit_jog_theta->setGeometry(QRect(330, 400, 61, 25));
        label_19 = new QLabel(centralwidget);
        label_19->setObjectName(QString::fromUtf8("label_19"));
        label_19->setGeometry(QRect(550, 220, 69, 31));
        btn_trace_enter = new QPushButton(centralwidget);
        btn_trace_enter->setObjectName(QString::fromUtf8("btn_trace_enter"));
        btn_trace_enter->setGeometry(QRect(610, 220, 111, 28));
        btn_trace_exit = new QPushButton(centralwidget);
        btn_trace_exit->setObjectName(QString::fromUtf8("btn_trace_exit"));
        btn_trace_exit->setGeometry(QRect(610, 260, 111, 28));
        label_20 = new QLabel(centralwidget);
        label_20->setObjectName(QString::fromUtf8("label_20"));
        label_20->setGeometry(QRect(600, 60, 111, 19));
        label_motion_mode = new QLabel(centralwidget);
        label_motion_mode->setObjectName(QString::fromUtf8("label_motion_mode"));
        label_motion_mode->setGeometry(QRect(720, 60, 271, 19));
        ledit_cmd_id = new QLineEdit(centralwidget);
        ledit_cmd_id->setObjectName(QString::fromUtf8("ledit_cmd_id"));
        ledit_cmd_id->setGeometry(QRect(350, 130, 113, 25));
        label_21 = new QLabel(centralwidget);
        label_21->setObjectName(QString::fromUtf8("label_21"));
        label_21->setGeometry(QRect(350, 100, 69, 19));
        btn_pause_resume = new QPushButton(centralwidget);
        btn_pause_resume->setObjectName(QString::fromUtf8("btn_pause_resume"));
        btn_pause_resume->setGeometry(QRect(590, 100, 141, 41));
        btn_stop = new QPushButton(centralwidget);
        btn_stop->setObjectName(QString::fromUtf8("btn_stop"));
        btn_stop->setGeometry(QRect(590, 150, 141, 41));
        btn_emergency_stop = new QPushButton(centralwidget);
        btn_emergency_stop->setObjectName(QString::fromUtf8("btn_emergency_stop"));
        btn_emergency_stop->setGeometry(QRect(750, 100, 211, 41));
        btn_error_reset = new QPushButton(centralwidget);
        btn_error_reset->setObjectName(QString::fromUtf8("btn_error_reset"));
        btn_error_reset->setGeometry(QRect(750, 150, 211, 41));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1027, 25));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        btn_ip_scan->setText(QCoreApplication::translate("MainWindow", "IP SCAN", nullptr));
        btn_connect_controller->setText(QCoreApplication::translate("MainWindow", "Connect", nullptr));
        btn_disconnect_controller->setText(QCoreApplication::translate("MainWindow", "Disconnect", nullptr));
        btn_trace_send->setText(QCoreApplication::translate("MainWindow", "SEND", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "File Name", nullptr));
        btn_trace_to_dat->setText(QCoreApplication::translate("MainWindow", "Trace to Dat", nullptr));
        btn_trace_read->setText(QCoreApplication::translate("MainWindow", "Trace Read", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Csv Name", nullptr));
        btn_trace_to_xlsx->setText(QCoreApplication::translate("MainWindow", "Dat to Csv", nullptr));
        btn_xlsx_to_dat->setText(QCoreApplication::translate("MainWindow", "Csv to Dat", nullptr));
        btn_thread_open->setText(QCoreApplication::translate("MainWindow", "Thread Open", nullptr));
        btn_thread_close->setText(QCoreApplication::translate("MainWindow", "Thread Close", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Connection", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Home", nullptr));
        btn_home->setText(QCoreApplication::translate("MainWindow", "Home", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "System State", nullptr));
        label_system_state->setText(QCoreApplication::translate("MainWindow", "Disconnected", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Direct Joint", nullptr));
        btn_direct_joint_enter->setText(QCoreApplication::translate("MainWindow", "ENTER", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Speed Level", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "1", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "2", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "3", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "4", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", "5", nullptr));
        btn_direct_joint_exit->setText(QCoreApplication::translate("MainWindow", "EXIT", nullptr));
        btn_direct_joint_send->setText(QCoreApplication::translate("MainWindow", "SEND", nullptr));
        label_13->setText(QCoreApplication::translate("MainWindow", "Cart Jog", nullptr));
        btn_jog_enter->setText(QCoreApplication::translate("MainWindow", "ENTER", nullptr));
        btn_jog_exit->setText(QCoreApplication::translate("MainWindow", "EXIT", nullptr));
        btn_jog_send->setText(QCoreApplication::translate("MainWindow", "SEND", nullptr));
        label_14->setText(QCoreApplication::translate("MainWindow", "x", nullptr));
        label_15->setText(QCoreApplication::translate("MainWindow", "y", nullptr));
        label_16->setText(QCoreApplication::translate("MainWindow", "z", nullptr));
        label_17->setText(QCoreApplication::translate("MainWindow", "\317\206", nullptr));
        label_18->setText(QCoreApplication::translate("MainWindow", "\316\270", nullptr));
        label_19->setText(QCoreApplication::translate("MainWindow", "Trace", nullptr));
        btn_trace_enter->setText(QCoreApplication::translate("MainWindow", "ENTER", nullptr));
        btn_trace_exit->setText(QCoreApplication::translate("MainWindow", "EXIT", nullptr));
        label_20->setText(QCoreApplication::translate("MainWindow", "Motion Mode", nullptr));
        label_motion_mode->setText(QCoreApplication::translate("MainWindow", "None", nullptr));
        label_21->setText(QCoreApplication::translate("MainWindow", "Cmd", nullptr));
        btn_pause_resume->setText(QCoreApplication::translate("MainWindow", "PAUSE", nullptr));
        btn_stop->setText(QCoreApplication::translate("MainWindow", "STOP", nullptr));
        btn_emergency_stop->setText(QCoreApplication::translate("MainWindow", "EMERGENCY", nullptr));
        btn_error_reset->setText(QCoreApplication::translate("MainWindow", "RESET", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
