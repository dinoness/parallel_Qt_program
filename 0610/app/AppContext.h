#pragma once

#include "zmotion/ZMotionDriver.h"
#include "protocol/CommandProtocol.h"
#include "protocol/ControllerProtocol.h"
#include "protocol/TraceProtocol.h"
#include "service/ConnectionService.h"
#include "service/MotionService.h"
#include "service/TrajectoryService.h"

class AppContext
{
public:
    AppContext();

    ZMotionDriver* driver();
    ControllerProtocol* protocol();
    CommandProtocol* commandProtocol();
    TraceProtocol* traceProtocol();
    ConnectionService* connectionService();
    MotionService* motionService();
    TrajectoryService* trajectoryService();

private:
    ZMotionDriver driver_;
    ControllerProtocol protocol_;
    CommandProtocol commandProtocol_;
    TraceProtocol traceProtocol_;
    ConnectionService connectionService_;
    MotionService motionService_;
    TrajectoryService trajectoryService_;
};
