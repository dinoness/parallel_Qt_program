#pragma once

#include "../core/Result.h"
#include "../zmotion/ZMotionDriver.h"

/// @brief 控制器 MODBUS 寄存器地址
namespace Reg {
    constexpr int EVENT_NORMAL = 100;   // 普通事件寄存器
}

/// @brief 在线命令写入协议
///
/// 封装控制器事件下发（如 TrajPrepare、Stop 等）。
class CommandWriter
{
public:
    explicit CommandWriter(ZMotionDriver* driver);

    /// @brief 发送事件通知到控制器
    Result sendEvent(int eventId);

private:
    ZMotionDriver* driver_;
};
