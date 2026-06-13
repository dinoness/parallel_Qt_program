#include "CartJogProtocol.h"
#include <QThread>
#include <QDebug>
#include <QElapsedTimer>

CartJogProtocol::CartJogProtocol(ZMotionDriver* driver)
    : driver_(driver)
{
}

// ===================================================================
// 事件寄存器等待
// ===================================================================

Result CartJogProtocol::waitForEventReg(uint16& stuckValue)
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

    return Result::fail(3501,
        QString("事件寄存器 %1 忙 (值=%2)，5s 超时")
            .arg(kRegEventLevel2).arg(stuckValue));
}

// ===================================================================
// 槽位等待
// ===================================================================

Result CartJogProtocol::waitSlotReady(int slot)
{
    constexpr int kTimeoutMs = 10000;

    QElapsedTimer timer;
    timer.start();

    while (true) {
        uint16 state = 0;
        Result ret = driver_->readModbusReg(kCartJogStatusBase + slot, state);
        if (!ret.ok) return ret;

        if (state != kDataUpdate) {
            return Result::success();
        }

        if (timer.elapsed() > kTimeoutMs) {
            return Result::fail(3510,
                QString("等待 Cart Jog 槽位 %1 释放超时 (当前状态=%2)")
                    .arg(slot).arg(state));
        }

        QThread::msleep(1);
    }
}

// ===================================================================
// 进入 / 退出
// ===================================================================

Result CartJogProtocol::enterCartJogMode()
{
    if (driver_ == nullptr) {
        return Result::fail(3502, "ZMotionDriver 未初始化");
    }

    writeIndex_ = 0;
    return Result::success();
}

Result CartJogProtocol::exitCartJogMode()
{
    if (driver_ == nullptr) {
        return Result::fail(3503, "ZMotionDriver 未初始化");
    }

    writeIndex_ = 0;
    return Result::success();
}

// ===================================================================
// 指令下发
// ===================================================================

Result CartJogProtocol::sendCartJogCommand(const float cmd[kCartJogCmdSize])
{
    if (driver_ == nullptr) {
        return Result::fail(3504, "ZMotionDriver 未初始化");
    }

    // 等待当前槽位空闲，防止覆写
    Result waitRet = waitSlotReady(writeIndex_);
    if (!waitRet.ok) return waitRet;

    int tableStart = kCartJogTableStart + writeIndex_ * kCartJogCmdSize;

    Result ret = driver_->setTable(tableStart, kCartJogCmdSize, cmd);
    if (!ret.ok) return ret;

    // 通知控制器有新指令
    ret = driver_->writeModbusReg(kCartJogStatusBase + writeIndex_, kDataUpdate);
    if (!ret.ok) return ret;

    writeIndex_ = (writeIndex_ + 1) % kCartJogBufferSize;

    // 指令下发完成后，下发 Cart Jog 运动事件
    ret = driver_->writeModbusReg(kRegEventLevel2,
                                  static_cast<uint16>(kEventCartJog));
    if (!ret.ok) return ret;

    return Result::success();
}

Result CartJogProtocol::readCartJogStatus(int slot, uint16& status)
{
    if (driver_ == nullptr) {
        return Result::fail(3505, "ZMotionDriver 未初始化");
    }

    return driver_->readModbusReg(kCartJogStatusBase + slot, status);
}
