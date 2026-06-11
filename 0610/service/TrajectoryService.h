#pragma once

#include <QObject>
#include <QString>

#include "../core/Result.h"
#include "../zmotion/ZMotionDriver.h"

class TrajectoryService : public QObject
{
    Q_OBJECT

public:
    explicit TrajectoryService(ZMotionDriver* driver, QObject* parent = nullptr);


signals:
    void connectionChanged(bool connected);

private:
    ZMotionDriver* driver_;
};