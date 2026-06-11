#pragma once

#include "zmotion/ZMotionDriver.h"
#include "service/ConnectionService.h"
#include "service/TrajectoryService.h"

class AppContext
{
public:
    AppContext();

    ZMotionDriver* driver();
    ConnectionService* connectionService();
    TrajectoryService* trajectoryService();

private:
    ZMotionDriver driver_;
    ConnectionService connectionService_;
    TrajectoryService trajectoryService_;
};
