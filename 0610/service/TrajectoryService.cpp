#include "TrajectoryService.h"

#include <QDebug>

#include "../motion/TrajectoryGenerator.h"
#include "../motion/TrajectoryTypes.h"

TrajectoryService::TrajectoryService(ZMotionDriver* driver,
                                     const QString& dataDir,
                                     QObject* parent)
    : QObject(parent),
      driver_(driver),
      file_(dataDir)
{
}

Result TrajectoryService::generateAndSave(const QString& fileName)
{
    QVector<TrajectoryPoint> points = TrajectoryGenerator::generateScrew();

    if (points.isEmpty()) {
        return Result::fail(2101, "轨迹生成失败，点数为空");
    }

    return file_.writeDat(fileName, points);
}

Result TrajectoryService::datToCsv(const QString& datFile, const QString& csvFile)
{
    QVector<TrajectoryPoint> points;
    Result ret = file_.readDat(datFile, points);
    if (!ret.ok) return ret;

    return file_.writeCsv(csvFile, points);
}

Result TrajectoryService::csvToDat(const QString& csvFile, const QString& datFile)
{
    QVector<TrajectoryPoint> points;
    Result ret = file_.readCsv(csvFile, points);
    if (!ret.ok) return ret;

    return file_.writeDat(datFile, points);
}

Result TrajectoryService::sendToController(const QString& fileName)
{
    if (driver_ == nullptr) {
        return Result::fail(2102, "ZMotionDriver 未初始化");
    }

    if (!driver_->isOpen()) {
        return Result::fail(2103, "控制器未连接，无法下发轨迹");
    }

    QVector<TrajectoryPoint> points;
    Result readRet = file_.readDat(fileName, points);
    if (!readRet.ok) return readRet;

    qDebug() << "sendToController: read" << points.size() << "points";

    Result ret = driver_->trigger();
    if (!ret.ok) return ret;

    int loopNum = 0;
    int totalPoints = points.size();
    float blockData[kDataBlockSize];

    while (loopNum * kDataGroupSize < totalPoints) {
        int curGroupId = loopNum % kDataGroupNum;

        // 等待控制器缓冲区有空位
        uint16 dataState;
        do {
            ret = driver_->readModbusReg(curGroupId, dataState);
            if (!ret.ok) {
                qDebug() << "readModbusReg failed in send loop:" << ret.message;
                return ret;
            }
        } while (dataState == kDataUpdate);

        // 填充数据块
        int baseIdx = loopNum * kDataGroupSize;
        for (int i = 0; i < kDataGroupSize; i++) {
            int ptIdx = baseIdx + i;
            if (ptIdx < totalPoints) {
                points[ptIdx].toArray(&blockData[i * kCmdSize]);
            } else {
                // 空指令：MOVE(0)
                blockData[i * kCmdSize] = 0;
                for (int j = 1; j < kCmdSize; j++) {
                    blockData[i * kCmdSize + j] = 0;
                }
            }
        }

        // 写入 TABLE
        int tableIdx = kDataStartIndex + curGroupId * kDataBlockSize;
        ret = driver_->setTable(tableIdx, kDataBlockSize, blockData);
        if (!ret.ok) {
            qDebug() << "setTable failed:" << ret.message;
            return ret;
        }

        // 标记数据已更新
        ret = driver_->writeModbusReg(curGroupId, kDataUpdate);
        if (!ret.ok) {
            qDebug() << "writeModbusReg failed:" << ret.message;
            return ret;
        }

        loopNum++;
        qDebug() << "sendToController loop:" << loopNum;
    }

    qDebug() << "sendToController over, total loops:" << loopNum;
    return Result::success();
}
