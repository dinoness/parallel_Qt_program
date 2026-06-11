#include "TableBufferWriter.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QThread>

// ── 协议常量（仅本文件可见） ──────────────────────────────
// MODBUS 寄存器：第 0~9 号寄存器对应 10 个缓冲组的状态
// TABLE 布局：从索引 kTableBase 开始，每组占 kBlockFloats 个 float
// 握手：kDataUpdate=1 表示已更新（等待控制器消费），非 1 即可写入

static constexpr int kTableBase     = 1000;                    // TABLE 起始索引
static constexpr int kBlockFloats   = kDataGroupSize * kCmdSize; // 700

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
    int totalGroups = (total + kDataGroupSize - 1) / kDataGroupSize;
    int loopNum = 0;
    float blockData[kBlockFloats];

    while (loopNum * kDataGroupSize < total) {
        // 检查取消
        if (options.isCancelled && options.isCancelled()) {
            return Result::fail(3002, "轨迹下发已取消");
        }

        int groupId = loopNum % kDataGroupNum;

        // 1. 等待缓冲区有空位
        ret = waitBufferReady(groupId, options.timeoutMs, options.isCancelled);
        if (!ret.ok) return ret;

        // 2. 打包数据
        int baseIdx = loopNum * kDataGroupSize;
        for (int i = 0; i < kDataGroupSize; i++) {
            int ptIdx = baseIdx + i;
            if (ptIdx < total) {
                points[ptIdx].toArray(&blockData[i * kCmdSize]);
            } else {
                // 末尾不足一组：补 MOVE(0)
                blockData[i * kCmdSize] = 1;
                for (int j = 1; j < kCmdSize; j++) {
                    blockData[i * kCmdSize + j] = 0;
                }
            }
        }

        // 3. 写入 TABLE
        ret = writeTableBlock(groupId, blockData, kBlockFloats);
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
    int addr = kTableBase + groupId * kBlockFloats;
    return driver_->setTable(addr, count, data);
}

Result TableBufferWriter::markBlockReady(int groupId)
{
    return driver_->writeModbusReg(groupId, kDataUpdate);
}
