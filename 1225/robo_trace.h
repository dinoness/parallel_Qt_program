#ifndef ROBO_TRACE_H
#define ROBO_TRACE_H


#include <math.h>
#include <QThread>
#include <QFile>
#include <QDataStream>
#include <QVector>
#include <QDebug>
#include <QDir>
#include "zaux.h"
#include "zmotion.h"
#include "xlsxdocument.h"


class robo_trace : public QThread
{
    Q_OBJECT

public:
    robo_trace();
    virtual void run();

    bool trace_to_file1(const QString& cus_file_name);
    bool trace_read(const QString& cus_file_name);
    bool trace_to_controller(ZMC_HANDLE g_handle, const QString& cus_file_name);
    bool trace_to_xlsx(const QString& dat_file_name, const QString& xlsx_file_name);
    bool xlsx_to_dat(const QString& dat_file_name, const QString& xlsx_file_name);

    bool xlsx_init(const QString& cus_file_name);
    bool xlsx_close();

    QString data_dir;

private:
    QXlsx::Document* p_xlsx = nullptr;

};



#endif // ROBO_TRACE_H
