#pragma once

#include <QObject>
#include <QString>

#include "../core/Result.h"
#include "../motion/TrajectoryFile.h"
#include "../zmotion/ZMotionDriver.h"

class TrajectoryService : public QObject
{
    Q_OBJECT

public:
    explicit TrajectoryService(ZMotionDriver* driver,
                               const QString& dataDir,
                               QObject* parent = nullptr);

    Result generateAndSave(const QString& fileName);
    Result sendToController(const QString& fileName);
    Result datToCsv(const QString& datFile, const QString& csvFile);
    Result csvToDat(const QString& csvFile, const QString& datFile);

private:
    ZMotionDriver* driver_;
    TrajectoryFile file_;
};
