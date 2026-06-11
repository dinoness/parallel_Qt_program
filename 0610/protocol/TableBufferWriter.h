#pragma once

#include <QVector>
#include <functional>

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

/// @brief  TABLE 缓冲区写入协议
///
/// 将轨迹数据通过 TABLE + MODBUS_REG 握手协议下发到控制器。
/// 协议细节（寄存器地址、TABLE 布局、握手常量）封装在此层。
class TableBufferWriter
{
public:
    explicit TableBufferWriter(ZMotionDriver* driver);

    /// @brief 发送全部轨迹点到控制器（含 ZAux_Trigger）
    Result sendAll(const QVector<TrajectoryPoint>& points,
                   const TrajectorySendOptions& options = TrajectorySendOptions());

private:
    /// @brief 等待指定缓冲区组的 MODBUS 状态变为非 Update（即有空位）
    Result waitBufferReady(int groupId,
                          int timeoutMs,
                          const std::function<bool()>& isCancelled);

    /// @brief 将一块 float 数据写入指定组的 TABLE 区域
    Result writeTableBlock(int groupId, const float* data, int count);

    /// @brief 将指定缓冲组标记为已更新
    Result markBlockReady(int groupId);

    ZMotionDriver* driver_;
};
