#pragma once

#include "zmotion/ZMotionDriver.h"
#include "protocol/ControllerProtocol.h"
#include "service/ConnectionService.h"
#include "service/TrajectoryService.h"

class AppContext
{
public:
    AppContext();

    ZMotionDriver* driver();
    ControllerProtocol* protocol();
    ConnectionService* connectionService();
    TrajectoryService* trajectoryService();

private:
    ZMotionDriver driver_;
    ControllerProtocol protocol_;
    ConnectionService connectionService_;
    TrajectoryService trajectoryService_;
};
