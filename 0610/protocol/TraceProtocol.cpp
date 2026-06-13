#include "TraceProtocol.h"
#include <QFile>
#include <QDataStream>
#include <QThread>
#include <QDebug>

#include "../core/ProtocolConstants.h"

TraceProtocol::TraceProtocol(ZMotionDriver* driver)
    : driver_(driver)
{
}

// ===================================================================
// 模式切换
// ===================================================================

Result TraceProtocol::enterTraceMode()
{
    if (driver_ == nullptr) {
        return Result::fail(3402, "ZMotionDriver 未初始化");
    }

    // 下发中不允许重复进入
    if (sending_) {
        return Result::fail(3410, "轨迹下发中，无法进入 Trace 模式");
    }

    return Result::success();
}

Result TraceProtocol::canExitTrace()
{
    if (driver_ == nullptr) {
        return Result::fail(3403, "ZMotionDriver 未初始化");
    }

    uint16_t state = 0;
    Result ret = driver_->readModbusReg(kRegSystemState, state);
    if (!ret.ok) return ret;

    if (state == kSysRunning) {
        return Result::fail(3404,
            QString("控制器正在运行轨迹 (状态寄存器 %1 = SYS_RUNNING)，请先停止轨迹")
                .arg(kRegSystemState));
    }

    return Result::success();
}

Result TraceProtocol::exitTraceMode()
{
    if (driver_ == nullptr) {
        return Result::fail(3420, "ZMotionDriver 未初始化");
    }

    sending_ = false;
    return Result::success();
}

// ===================================================================
// 轨迹下发
// ===================================================================

bool TraceProtocol::isSending() const
{
    return sending_;
}

Result TraceProtocol::sendTrajectory(const QString& datFilePath,
                                      int totalPoints,
                                      const TrajectorySendOptions& options)
{
    sending_ = true;

    // 1. 校验系统状态：仅 kSysReady 允许下发
    uint16_t sysState = 0;
    Result ret = driver_->readModbusReg(kRegSystemState, sysState);
    if (!ret.ok) {
        sending_ = false;
        return ret;
    }

    if (sysState != kSysReady) {
        sending_ = false;
        return Result::fail(3415,
            QString("系统状态不允许下发 Trace (当前=%1, 需要=%2)")
                .arg(sysState).arg(kSysReady));
    }

    // 2. 将所有轨迹状态寄存器统一置为 kDataBlank
    for (int i = 0; i < kTrajGroupNum; ++i) {
        ret = driver_->writeModbusReg(kRegTrajStatusBase + i, kDataBlank);
        if (!ret.ok) {
            sending_ = false;
            return ret;
        }
    }

    // 3. 打开文件（流式读取，不在内存中全量加载）
    QFile file(datFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        sending_ = false;
        return Result::fail(3416,
            QString("无法打开轨迹文件：%1，错误：%2")
                .arg(datFilePath, file.errorString()));
    }

    QDataStream in(&file);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    // 4. Trigger
    ret = driver_->trigger();
    if (!ret.ok) {
        sending_ = false;
        file.close();
        return ret;
    }

    // 5. 下发事件：开始轨迹运动
    ret = driver_->writeModbusReg(kRegEventLevel2,
                                  static_cast<uint16_t>(kEventTraj));
    if (!ret.ok) {
        sending_ = false;
        file.close();
        return ret;
    }

    // 6. 循环：按组流式读取并下发
    int totalGroups = (totalPoints + kTrajGroupSize - 1) / kTrajGroupSize;
    int loopNum = 0;
    float blockData[kTrajBlockSize];

    while (loopNum * kTrajGroupSize < totalPoints) {
        // 检查取消
        if (options.isCancelled && options.isCancelled()) {
            sending_ = false;
            file.close();
            return Result::fail(3405, "轨迹下发已取消");
        }

        // 检查暂停：阻塞循环直到恢复或取消
        if (options.isPaused) {
            while (options.isPaused()) {
                if (options.isCancelled && options.isCancelled()) {
                    sending_ = false;
                    file.close();
                    return Result::fail(3405, "轨迹下发已取消");
                }
                QThread::msleep(100);
            }
        }

        int groupId = loopNum % kTrajGroupNum;

        // 等待缓冲区有空位（无超时，仅取消可中断）
        ret = waitBufferReady(groupId, options.isCancelled);
        if (!ret.ok) {
            sending_ = false;
            file.close();
            return ret;
        }

        // 从文件流式读取一组数据
        int count = 0;
        for (int i = 0; i < kTrajGroupSize; ++i) {
            int ptIdx = loopNum * kTrajGroupSize + i;
            float* dest = &blockData[i * kTrajCmdSize];

            if (ptIdx < totalPoints && !in.atEnd()) {
                for (int j = 0; j < kTrajCmdSize; ++j) {
                    in >> dest[j];
                    if (in.status() != QDataStream::Ok) {
                        // 读取出错：填充 MOVE(0)
                        dest[0] = kCmdMove;
                        for (int k = 1; k < kTrajCmdSize; ++k) {
                            dest[k] = 0;
                        }
                        in.resetStatus();
                        break;
                    }
                }
                ++count;
            } else {
                // 末尾不足一组：补 MOVE(0)
                dest[0] = kCmdMove;
                for (int k = 1; k < kTrajCmdSize; ++k) {
                    dest[k] = 0;
                }
            }
        }

        if (count == 0 && loopNum * kTrajGroupSize >= totalPoints) {
            break;
        }

        // 写入 TABLE
        int tableAddr = kTrajTableStart + groupId * kTrajBlockSize;
        ret = driver_->setTable(tableAddr, kTrajBlockSize, blockData);
        if (!ret.ok) {
            sending_ = false;
            file.close();
            return ret;
        }

        // 标记已更新
        ret = driver_->writeModbusReg(kRegTrajStatusBase + groupId, kDataUpdate);
        if (!ret.ok) {
            sending_ = false;
            file.close();
            return ret;
        }

        loopNum++;

        // 进度回调
        if (options.onProgress) {
            options.onProgress(loopNum, totalGroups);
        }

        qDebug() << "TraceProtocol sendTrajectory loop:" << loopNum;
    }

    file.close();
    sending_ = false;

    qDebug() << "TraceProtocol sendTrajectory done, total loops:" << loopNum;
    return Result::success();
}

// ===================================================================
// 轨迹下发内部方法
// ===================================================================

Result TraceProtocol::waitBufferReady(int groupId,
                                       const std::function<bool()>& isCancelled)
{
    uint16_t state = 0;
    int addr = kRegTrajStatusBase + groupId;

    while (true) {
        if (isCancelled && isCancelled()) {
            return Result::fail(3406, "轨迹下发已取消");
        }

        Result ret = driver_->readModbusReg(addr, state);
        if (!ret.ok) return ret;

        if (state != kDataUpdate) {
            return Result::success();
        }

        // 无超时：允许堵塞等待，间隔 1ms 轮询
        QThread::msleep(1);
    }
}
