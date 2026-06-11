#include "AppContext.h"

AppContext::AppContext()
    : driver_(),
      connectionService_(&driver_),
      trajectoryService_(&driver_, "../../trace_data/")
{
}

ZMotionDriver* AppContext::driver()
{
    return &driver_;
}

ConnectionService* AppContext::connectionService()
{
    return &connectionService_;
}

TrajectoryService* AppContext::trajectoryService()
{
    return &trajectoryService_;
}