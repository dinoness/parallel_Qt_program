#include "TableBufferWriter.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QThread>

#include "../core/ProtocolConstants.h"

// ── 协议常量 ──────────────────────────────────────────
// 使用 ProtocolConstants.h 中的 Traj 定义
// TABLE 布局: kTrajTableStart 开始, 每组 kTrajBlockSize 个 float
// MODBUS 寄存器: 0~9 (kTrajGroupNum 个), 对应 10 个缓冲组
// 握手: kDataUpdate=1 表示已更新

TableBufferWriter::TableBufferWriter(ZMotionDriver* driver)
    : driver_(driver)
{
}

Result TableBufferWriter::sendAll(const QVector<TrajectoryPoint>& points,
                                   const TrajectorySendOptions& options)
{
    Result ret = driver_->trigger();
    if (!ret.ok) return ret;

    int total = points.size();
    int totalGroups = (total + kTrajGroupSize - 1) / kTrajGroupSize;
    int loopNum = 0;
    float blockData[kTrajBlockSize];

    while (loopNum * kTrajGroupSize < total) {
        // 检查取消
        if (options.isCancelled && options.isCancelled()) {
            return Result::fail(3002, "轨迹下发已取消");
        }

        int groupId = loopNum % kTrajGroupNum;

        // 1. 等待缓冲区有空位
        ret = waitBufferReady(groupId, options.timeoutMs, options.isCancelled);
        if (!ret.ok) return ret;

        // 2. 打包数据
        int baseIdx = loopNum * kTrajGroupSize;
        for (int i = 0; i < kTrajGroupSize; i++) {
            int ptIdx = baseIdx + i;
            if (ptIdx < total) {
                points[ptIdx].toArray(&blockData[i * kTrajCmdSize]);
            } else {
                // 末尾不足一组：补 MOVE(0)
                blockData[i * kTrajCmdSize] = 1;
                for (int j = 1; j < kTrajCmdSize; j++) {
                    blockData[i * kTrajCmdSize + j] = 0;
                }
            }
        }

        // 3. 写入 TABLE
        ret = writeTableBlock(groupId, blockData, kTrajBlockSize);
        if (!ret.ok) return ret;

        // 4. 标记已更新
        ret = markBlockReady(groupId);
        if (!ret.ok) return ret;

        loopNum++;

        // 5. 进度回调
        if (options.onProgress) {
            options.onProgress(loopNum, totalGroups);
        }

        qDebug() << "TableBufferWriter loop:" << loopNum;
    }

    qDebug() << "TableBufferWriter done, total loops:" << loopNum;
    return Result::success();
}

Result TableBufferWriter::waitBufferReady(int groupId,
                                           int timeoutMs,
                                           const std::function<bool()>& isCancelled)
{
    uint16 state = 0;

    QElapsedTimer timer;
    timer.start();

    while (true) {
        if (isCancelled && isCancelled()) {
            return Result::fail(3002, "轨迹下发已取消");
        }

        Result ret = driver_->readModbusReg(groupId, state);
        if (!ret.ok) {
            return ret;
        }

        if (state != kDataUpdate) {
            return Result::success();
        }

        if (timer.elapsed() > timeoutMs) {
            return Result::fail(
                3001,
                QString("等待缓冲组 %1 释放超时").arg(groupId));
        }

        QThread::msleep(1);
    }
}

Result TableBufferWriter::writeTableBlock(int groupId, const float* data, int count)
{
    int addr = kTrajTableStart + groupId * kTrajBlockSize;
    return driver_->setTable(addr, count, data);
}

Result TableBufferWriter::markBlockReady(int groupId)
{
    return driver_->writeModbusReg(groupId, kDataUpdate);
}
