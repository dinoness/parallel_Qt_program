#include "TraceProtocol.h"
#include <QThread>
#include <QDebug>
#include <QElapsedTimer>

TraceProtocol::TraceProtocol(ZMotionDriver* driver)
    : driver_(driver)
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

Result TraceProtocol::sendTrajectory(const QVector<TrajectoryPoint>& points,
                                      const TrajectorySendOptions& options)
{
    sending_ = true;

    Result ret = driver_->trigger();
    if (!ret.ok) {
        sending_ = false;
        return ret;
    }

    int total = points.size();
    int totalGroups = (total + kTrajGroupSize - 1) / kTrajGroupSize;
    int loopNum = 0;
    float blockData[kTrajBlockSize];

    while (loopNum * kTrajGroupSize < total) {
        // 检查取消
        if (options.isCancelled && options.isCancelled()) {
            sending_ = false;
            return Result::fail(3405, "轨迹下发已取消");
        }

        int groupId = loopNum % kTrajGroupNum;

        // 1. 等待缓冲区有空位
        ret = waitBufferReady(groupId, options.timeoutMs, options.isCancelled);
        if (!ret.ok) {
            sending_ = false;
            return ret;
        }

        // 2. 打包数据
        int baseIdx = loopNum * kTrajGroupSize;
        for (int i = 0; i < kTrajGroupSize; i++) {
            int ptIdx = baseIdx + i;
            if (ptIdx < total) {
                points[ptIdx].toArray(&blockData[i * kTrajCmdSize]);
            } else {
                // 末尾不足一组：补 MOVE(0)
                blockData[i * kTrajCmdSize] = kCmdMove;
                for (int j = 1; j < kTrajCmdSize; j++) {
                    blockData[i * kTrajCmdSize + j] = 0;
                }
            }
        }

        // 3. 写入 TABLE
        ret = writeTableBlock(groupId, blockData, kTrajBlockSize);
        if (!ret.ok) {
            sending_ = false;
            return ret;
        }

        // 4. 标记已更新
        ret = markBlockReady(groupId);
        if (!ret.ok) {
            sending_ = false;
            return ret;
        }

        loopNum++;

        // 5. 进度回调
        if (options.onProgress) {
            options.onProgress(loopNum, totalGroups);
        }

        qDebug() << "TraceProtocol sendTrajectory loop:" << loopNum;
    }

    sending_ = false;

    qDebug() << "TraceProtocol sendTrajectory done, total loops:" << loopNum;
    return Result::success();
}

// ===================================================================
// 轨迹下发内部方法
// ===================================================================

Result TraceProtocol::waitBufferReady(int groupId,
                                       int timeoutMs,
                                       const std::function<bool()>& isCancelled)
{
    uint16 state = 0;
    int addr = kTrajStatusBase + groupId;

    QElapsedTimer timer;
    timer.start();

    while (true) {
        if (isCancelled && isCancelled()) {
            return Result::fail(3406, "轨迹下发已取消");
        }

        Result ret = driver_->readModbusReg(addr, state);
        if (!ret.ok) return ret;

        if (state != kDataUpdate) {
            return Result::success();
        }

        if (timer.elapsed() > timeoutMs) {
            return Result::fail(3407,
                QString("等待缓冲组 %1 (MODBUS %2) 释放超时")
                    .arg(groupId).arg(addr));
        }

        QThread::msleep(1);
    }
}

Result TraceProtocol::writeTableBlock(int groupId, const float* data, int count)
{
    int addr = kTrajTableStart + groupId * kTrajBlockSize;
    return driver_->setTable(addr, count, data);
}

Result TraceProtocol::markBlockReady(int groupId)
{
    int addr = kTrajStatusBase + groupId;
    return driver_->writeModbusReg(addr, kDataUpdate);
}
