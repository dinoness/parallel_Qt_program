#include "TraceProtocol.h"
#include <QThread>
#include <QDebug>

TraceProtocol::TraceProtocol(ZMotionDriver* driver)
    : driver_(driver)
    , tableWriter_(driver)
    , commandWriter_(driver)
{
}

// ===================================================================
// 事件寄存器等待
// ===================================================================

Result TraceProtocol::waitForEventReg(uint16& stuckValue)
{
    constexpr int kTimeoutMs  = 5000;
    constexpr int kIntervalMs = 1000;

    for (int elapsed = 0; elapsed < kTimeoutMs; elapsed += kIntervalMs) {
        uint16 val = 0;
        Result ret = driver_->readModbusReg(kEventReg, val);
        if (!ret.ok) return ret;

        if (val == 0) {
            return Result::success();
        }

        stuckValue = val;

        if (elapsed + kIntervalMs < kTimeoutMs) {
            QThread::msleep(kIntervalMs);
        }
    }

    return Result::fail(3401,
        QString("事件寄存器 %1 忙 (值=%2)，5s 超时")
            .arg(kEventReg).arg(stuckValue));
}

// ===================================================================
// 模式切换
// ===================================================================

Result TraceProtocol::enterTraceMode()
{
    if (driver_ == nullptr) {
        return Result::fail(3402, "ZMotionDriver 未初始化");
    }

    // 下发中不允许发送新事件
    if (sending_) {
        return Result::fail(3410, "轨迹下发中，无法发送事件");
    }

    // 等待事件寄存器清零
    uint16 stuckVal = 0;
    Result waitRet = waitForEventReg(stuckVal);
    if (!waitRet.ok) {
        qDebug() << "enterTraceMode: waitForEventReg failed:" << waitRet.message;
        return waitRet;
    }

    return driver_->writeModbusReg(kEventReg,
                                   static_cast<uint16>(kEventTraj));
}

Result TraceProtocol::canExitTrace()
{
    if (driver_ == nullptr) {
        return Result::fail(3403, "ZMotionDriver 未初始化");
    }

    uint16 state = 0;
    Result ret = driver_->readModbusReg(kSystemStateReg, state);
    if (!ret.ok) return ret;

    if (state == kSysRunning) {
        return Result::fail(3404,
            QString("控制器正在运行轨迹 (状态寄存器 %1 = SYS_RUNNING)，请先停止轨迹")
                .arg(kSystemStateReg));
    }

    return Result::success();
}

// ===================================================================
// 轨迹下发
// ===================================================================

bool TraceProtocol::isSending() const
{
    return sending_;
}

TableBufferWriter* TraceProtocol::tableWriter()
{
    return &tableWriter_;
}

CommandWriter* TraceProtocol::commandWriter()
{
    return &commandWriter_;
}

Result TraceProtocol::sendTrajectory(const QVector<TrajectoryPoint>& points,
                                      const TrajectorySendOptions& options)
{
    // 下发过程中不下发新的事件
    sending_ = true;

    Result ret = tableWriter_.sendAll(points, options);
    if (!ret.ok) {
        sending_ = false;
        return ret;
    }

    sending_ = false;
    return Result::success();
}
