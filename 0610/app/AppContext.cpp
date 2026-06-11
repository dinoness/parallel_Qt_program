#include "AppContext.h"

AppContext::AppContext()
    : driver_(),
      connectionService_(&driver_)
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