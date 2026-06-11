#include "AppContext.h"

AppContext::AppContext()
    : driver_(),
      protocol_(&driver_),
      connectionService_(&driver_),
      trajectoryService_(&driver_, &protocol_, "../../trace_data/")
{
}

ZMotionDriver* AppContext::driver()
{
    return &driver_;
}

ControllerProtocol* AppContext::protocol()
{
    return &protocol_;
}

ConnectionService* AppContext::connectionService()
{
    return &connectionService_;
}

TrajectoryService* AppContext::trajectoryService()
{
    return &trajectoryService_;
}
