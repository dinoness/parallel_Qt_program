#include "JointProtocol.h"
#include <QThread>
#include <QDebug>

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

    return Result::fail(3205,
        QString("事件寄存器 %1 忙 (值=%2)，5s 超时")
            .arg(kEventReg).arg(stuckValue));
}

// ===================================================================
// 进入 / 退出
// ===================================================================

Result JointProtocol::enterJointMode()
{
    if (driver_ == nullptr) {
        return Result::fail(3201, "ZMotionDriver 未初始化");
    }

    // 等待事件寄存器清零
    uint16 stuckVal = 0;
    Result waitRet = waitForEventReg(stuckVal);
    if (!waitRet.ok) {
        qDebug() << "enterJointMode: waitForEventReg failed:" << waitRet.message;
        return waitRet;
    }

    writeIndex_ = 0;

    return driver_->writeModbusReg(kEventReg,
                                   static_cast<uint16>(kEventJoint));
}

Result JointProtocol::exitJointMode()
{
    if (driver_ == nullptr) {
        return Result::fail(3202, "ZMotionDriver 未初始化");
    }

    uint16 stuckVal = 0;
    Result waitRet = waitForEventReg(stuckVal);
    if (!waitRet.ok) {
        qDebug() << "exitJointMode: waitForEventReg failed:" << waitRet.message;
        return waitRet;
    }

    return driver_->writeModbusReg(kEventReg,
                                   static_cast<uint16>(kEventJointDone));
}

// ===================================================================
// 指令下发
// ===================================================================

Result JointProtocol::sendJointCommand(const float cmd[kJointCmdSize])
{
    if (driver_ == nullptr) {
        return Result::fail(3203, "ZMotionDriver 未初始化");
    }

    int tableStart = kJointTableStart + writeIndex_ * kJointCmdSize;

    Result ret = driver_->setTable(tableStart, kJointCmdSize, cmd);
    if (!ret.ok) return ret;

    // 通知控制器有新指令: 写状态寄存器 (0=新数据就绪)
    ret = driver_->writeModbusReg(kJointStatusBase + writeIndex_, 0);
    if (!ret.ok) return ret;

    writeIndex_ = (writeIndex_ + 1) % kJointBufferSize;

    return Result::success();
}

Result JointProtocol::readJointStatus(int slot, uint16& status)
{
    if (driver_ == nullptr) {
        return Result::fail(3204, "ZMotionDriver 未初始化");
    }

    return driver_->readModbusReg(kJointStatusBase + slot, status);
}
