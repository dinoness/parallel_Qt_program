#pragma once

#include "../core/ProtocolConstants.h"
#include "../core/Result.h"
#include "../motion/TrajectoryTypes.h"
#include "../zmotion/ZMotionDriver.h"
#include "CommandWriter.h"
#include "TableBufferWriter.h"

/// @brief 轨迹模式协议 — 封装 Trace 模式的所有协议操作
///
/// 模式切换: MODBUS 寄存器 90/92。
/// 轨迹下发: TABLE 缓冲 + TrajPrepare 事件。
/// 下发过程中阻止发送新的事件。
class TraceProtocol
{
public:
    explicit TraceProtocol(ZMotionDriver* driver);

    // ── 模式切换 ──────────────────────────────────────
    /// @brief 进入轨迹模式 — 发送 EVENT_TRAJ 到 MODBUS[92]
    Result enterTraceMode();

    /// @brief 检查是否允许退出 — 读取系统状态，SYS_RUNNING 时返回失败
    Result canExitTrace();

    // ── 轨迹下发 ──────────────────────────────────────
    TableBufferWriter* tableWriter();
    CommandWriter*     commandWriter();

    /// @brief 下发轨迹 + 发送 TrajPrepare 事件
    /// @note 下发过程中不下发新的事件
    Result sendTrajectory(const QVector<TrajectoryPoint>& points,
                          const TrajectorySendOptions& options = TrajectorySendOptions());

    /// @brief 是否正在下发轨迹
    bool isSending() const;

private:
    /// @brief 事件寄存器写入前等待 — 轮询直到寄存器为 0 或超时 (5s)
    Result waitForEventReg(uint16& stuckValue);

    ZMotionDriver*    driver_;
    TableBufferWriter tableWriter_;
    CommandWriter     commandWriter_;
    bool              sending_ = false;
};
