#pragma once

#include <QString>
#include <QVector>

#include "TrajectoryTypes.h"
#include "../core/Result.h"

class TrajectoryFile
{
public:
    TrajectoryFile(const QString& dataDir);

    Result writeDat(const QString& fileName, const QVector<TrajectoryPoint>& points);
    Result readDat(const QString& fileName, QVector<TrajectoryPoint>& points);

    Result writeCsv(const QString& fileName, const QVector<TrajectoryPoint>& points);
    Result readCsv(const QString& fileName, QVector<TrajectoryPoint>& points);

private:
    QString dataDir_;
};
