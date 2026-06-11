#pragma once

#include <QVector>

#include "../core/Result.h"
#include "../motion/TrajectoryTypes.h"
#include "../zmotion/ZMotionDriver.h"
#include "CommandWriter.h"
#include "TableBufferWriter.h"

/// @brief 事件 ID — 后续迁移到 core/CommandId.h
namespace EventId {
    constexpr int TrajPrepare = 0x10;
}


/// @brief 控制器协议总入口
///
/// 聚合各子协议对象（TABLE 缓冲、在线命令等），提供面向业务的上层接口。
/// Service 层通过本类下发轨迹，无需关心底层寄存器/TABLE/握手细节。
class ControllerProtocol
{
public:
    explicit ControllerProtocol(ZMotionDriver* driver);

    TableBufferWriter* tableWriter();
    CommandWriter*     commandWriter();

    /// @brief 下发轨迹 + 发送 TrajPrepare 事件
    Result sendTrajectory(const QVector<TrajectoryPoint>& points,
                          const TrajectorySendOptions& options = TrajectorySendOptions());

private:
    TableBufferWriter tableWriter_;
    CommandWriter     commandWriter_;
};
