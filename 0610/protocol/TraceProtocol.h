#pragma once

#include <QVector>
#include <functional>

#include "../core/ProtocolConstants.h"
#include "../core/Result.h"
#include "../motion/TrajectoryTypes.h"
#include "../zmotion/ZMotionDriver.h"

/// @brief 轨迹下发选项
struct TrajectorySendOptions
{
    int timeoutMs = 10000;

    /// @brief 进度回调：参数(已发送组数, 总组数)
    std::function<void(int, int)> onProgress;

    /// @brief 取消检查：返回 true 表示请求取消
    std::function<bool()> isCancelled;
};

/// @brief 轨迹模式协议 — 封装 Trace 模式的所有协议操作
///
/// 模式切换: MODBUS 寄存器 90/92。
/// 轨迹下发: TABLE[kTrajTableStart] + MODBUS[kRegTrajStatusBase] 缓冲握手。
/// 下发过程中阻止发送新的事件。
class TraceProtocol
{
public:
    explicit TraceProtocol(ZMotionDriver* driver);

    // ── 模式切换 ──────────────────────────────────────
    /// @brief 进入轨迹模式 — 仅验证状态，不下发事件
    Result enterTraceMode();

    /// @brief 检查是否允许退出 — 读取系统状态，SYS_RUNNING 时返回失败
    Result canExitTrace();

    // ── 轨迹下发 ──────────────────────────────────────
    /// @brief 下发轨迹 (含 ZAux_Trigger)，第一组数据下发后写入事件
    Result sendTrajectory(const QVector<TrajectoryPoint>& points,
                          const TrajectorySendOptions& options = TrajectorySendOptions());

    /// @brief 是否正在下发轨迹
    bool isSending() const;

private:
    /// @brief 事件寄存器写入前等待 — 轮询直到寄存器为 0 或超时 (5s)
    Result waitForEventReg(uint16& stuckValue);

    // ── 轨迹下发内部方法 ──────────────────────────────
    /// @brief 等待指定缓冲组状态为可用 (非 kDataUpdate)
    Result waitBufferReady(int groupId, int timeoutMs,
                           const std::function<bool()>& isCancelled);

    /// @brief 将一块 float 数据写入指定组的 TABLE 区域
    Result writeTableBlock(int groupId, const float* data, int count);

    /// @brief 将指定缓冲组标记为已更新 (kDataUpdate)
    Result markBlockReady(int groupId);

    ZMotionDriver* driver_;
    bool           sending_ = false;
};
