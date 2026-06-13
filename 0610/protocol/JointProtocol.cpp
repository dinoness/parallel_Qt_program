#include "JointProtocol.h"
#include <QThread>
#include <QDebug>
#include <QElapsedTimer>

JointProtocol::JointProtocol(ZMotionDriver* driver)
    : driver_(driver)
{
}

// ===================================================================
// 事件寄存器等待
// ===================================================================

Result JointProtocol::waitForEventReg(uint16& stuckValue)
{
    constexpr int kTimeoutMs  = 5000;
    constexpr int kIntervalMs = 1000;

    for (int elapsed = 0; elapsed < kTimeoutMs; elapsed += kIntervalMs) {
        uint16 val = 0;
        Result ret = driver_->readModbusReg(kRegEventLevel2, val);
        if (!ret.ok) return ret;

        if (val == 0) {
            return Result::success();
        }

        stuckValue = val;

        if (elapsed + kIntervalMs < kTimeoutMs) {
            QThread::msleep(kIntervalMs);
        }
    }

    return Result::fail(3205,
        QString("事件寄存器 %1 忙 (值=%2)，5s 超时")
            .arg(kRegEventLevel2).arg(stuckValue));
}

// ===================================================================
// 槽位等待
// ===================================================================

Result JointProtocol::waitSlotReady(int slot)
{
    constexpr int kTimeoutMs = 10000;

    QElapsedTimer timer;
    timer.start();

    while (true) {
        uint16 state = 0;
        Result ret = driver_->readModbusReg(kJointStatusBase + slot, state);
        if (!ret.ok) return ret;

        if (state != kDataUpdate) {
            return Result::success();
        }

        if (timer.elapsed() > kTimeoutMs) {
            return Result::fail(3210,
                QString("等待 Joint 槽位 %1 释放超时 (当前状态=%2)")
                    .arg(slot).arg(state));
        }

        QThread::msleep(1);
    }
}

// ===================================================================
// 进入 / 退出
// ===================================================================

Result JointProtocol::enterJointMode()
{
    if (driver_ == nullptr) {
        return Result::fail(3201, "ZMotionDriver 未初始化");
    }

    writeIndex_ = 0;
    return Result::success();
}

Result JointProtocol::exitJointMode()
{
    if (driver_ == nullptr) {
        return Result::fail(3202, "ZMotionDriver 未初始化");
    }

    writeIndex_ = 0;
    return Result::success();
}

// ===================================================================
// 指令下发
// ===================================================================

Result JointProtocol::sendJointCommand(const float cmd[kJointCmdSize])
{
    if (driver_ == nullptr) {
        return Result::fail(3203, "ZMotionDriver 未初始化");
    }

    // 等待当前槽位空闲，防止覆写
    Result waitRet = waitSlotReady(writeIndex_);
    if (!waitRet.ok) return waitRet;

    int tableStart = kJointTableStart + writeIndex_ * kJointCmdSize;

    Result ret = driver_->setTable(tableStart, kJointCmdSize, cmd);
    if (!ret.ok) return ret;

    // 通知控制器有新指令
    ret = driver_->writeModbusReg(kJointStatusBase + writeIndex_, kDataUpdate);
    if (!ret.ok) return ret;

    writeIndex_ = (writeIndex_ + 1) % kJointBufferSize;

    // 指令下发完成后，下发关节运动事件
    ret = driver_->writeModbusReg(kRegEventLevel2,
                                  static_cast<uint16>(kEventJoint));
    if (!ret.ok) return ret;

    return Result::success();
}

Result JointProtocol::readJointStatus(int slot, uint16& status)
{
    if (driver_ == nullptr) {
        return Result::fail(3204, "ZMotionDriver 未初始化");
    }

    return driver_->readModbusReg(kJointStatusBase + slot, status);
}
