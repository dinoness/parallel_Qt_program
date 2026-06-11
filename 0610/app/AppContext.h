#pragma once

#include "zmotion/ZMotionDriver.h"
#include "service/ConnectionService.h"

class AppContext
{
public:
    AppContext();

    ZMotionDriver* driver();
    ConnectionService* connectionService();

private:
    ZMotionDriver driver_;
    ConnectionService connectionService_;
};