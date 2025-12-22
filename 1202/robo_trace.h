#ifndef ROBO_TRACE_H
#define ROBO_TRACE_H

#include <QThread>
#include <QFile>
#include <QDataStream>
#include <QVector>
#include <math.h>

class robo_trace : public QThread
{
    Q_OBJECT

public:
    robo_trace();
    virtual void run();

    bool trace_to_file1(const QString& fileName);

private:

};



#endif // ROBO_TRACE_H
