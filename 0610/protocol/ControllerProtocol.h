#pragma once

#include "../zmotion/ZMotionDriver.h"

/// @brief 控制器协议总入口（已废弃轨迹下发功能，迁移至 TraceProtocol）
class ControllerProtocol
{
public:
    explicit ControllerProtocol(ZMotionDriver* driver);

private:
    ZMotionDriver* driver_;
};
