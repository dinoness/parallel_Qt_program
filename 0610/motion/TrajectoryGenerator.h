#pragma once

#include <QVector>

#include "TrajectoryTypes.h"

class TrajectoryGenerator
{
public:
    static QVector<TrajectoryPoint> generateScrew();
};
