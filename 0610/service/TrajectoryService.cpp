#include "TrajectoryService.h"

#include <QDebug>
#include <QMetaObject>

#include "../motion/TrajectoryGenerator.h"

TrajectoryService::TrajectoryService(ZMotionDriver* driver,
                                     TraceProtocol* protocol,
                                     const QString& dataDir,
                                     QObject* parent)
    : QObject(parent),
      driver_(driver),
      protocol_(protocol),
      file_(dataDir)
{
}

TrajectoryService::~TrajectoryService()
{
    cancelSendTrajectory();

    if (sendThread_ != nullptr) {
        sendThread_->quit();
        sendThread_->wait(3000);
    }

    cleanupSendThread();
}

bool TrajectoryService::isSending() const
{
    return sending_;
}

// ── 文件操作 ──────────────────────────────────────────────────────

Result TrajectoryService::generateAndSave(const QString& fileName)
{
    QVector<TrajectoryPoint> points = TrajectoryGenerator::generateScrew();

    if (points.isEmpty()) {
        return Result::fail(2101, "轨迹生成失败，点数为空");
    }

    return file_.writeDat(fileName, points);
}

Result TrajectoryService::loadPoints(const QString& fileName,
                                      QVector<TrajectoryPoint>& points)
{
    return file_.readDat(fileName, points);
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

// ── 异步下发 ──────────────────────────────────────────────────────

Result TrajectoryService::startSendTrajectoryAsync(const QString& fileName)
{
    if (protocol_ == nullptr) {
        return Result::fail(3301, "TraceProtocol 未初始化");
    }

    if (sending_ || sendThread_ != nullptr || sendWorker_ != nullptr) {
        return Result::fail(3303, "当前已有轨迹正在下发");
    }

    // 先快速读取文件以获取总点数（用于进度计算），
    // 实际下发时 Protocol 层会再次打开文件流式读取
    QVector<TrajectoryPoint> points;
    Result ret = file_.readDat(fileName, points);
    if (!ret.ok) return ret;

    if (points.isEmpty()) {
        return Result::fail(3302, "轨迹数据为空，无法下发");
    }

    int totalPoints = points.size();
    // 释放内存：文件已存在，下发时流式读取
    points.clear();

    // 构建完整 .dat 文件路径
    QString datFilePath = file_.datPath(fileName);

    sending_ = true;
    emit sendingStateChanged(true);

    sendThread_ = new QThread();
    sendWorker_ = new TrajectorySendWorker(protocol_);

    sendWorker_->moveToThread(sendThread_);

    connect(sendThread_, &QThread::started,
            sendWorker_, [this, datFilePath, totalPoints]() {
                sendWorker_->startSend(datFilePath, totalPoints);
            });

    connect(sendWorker_, &TrajectorySendWorker::progressChanged,
            this, &TrajectoryService::sendProgressChanged);

    connect(sendWorker_, &TrajectorySendWorker::finished,
            this, [this](const Result& result) {
                emit sendFinished(result);

                if (sendThread_ != nullptr) {
                    sendThread_->quit();
                }
            });

    connect(sendThread_, &QThread::finished,
            sendWorker_, &QObject::deleteLater);

    connect(sendThread_, &QThread::finished,
            sendThread_, &QObject::deleteLater);

    connect(sendThread_, &QThread::finished,
            this, [this]() {
                sendWorker_ = nullptr;
                sendThread_ = nullptr;
                sending_ = false;
                emit sendingStateChanged(false);
            });

    sendThread_->start();

    return Result::success();
}

void TrajectoryService::cancelSendTrajectory()
{
    if (sendWorker_ != nullptr) {
        QMetaObject::invokeMethod(sendWorker_,
                                  "cancel",
                                  Qt::QueuedConnection);
    }
}

void TrajectoryService::pauseSendTrajectory()
{
    if (sendWorker_ != nullptr) {
        QMetaObject::invokeMethod(sendWorker_,
                                  "pause",
                                  Qt::QueuedConnection);
    }
}

void TrajectoryService::resumeSendTrajectory()
{
    if (sendWorker_ != nullptr) {
        QMetaObject::invokeMethod(sendWorker_,
                                  "resume",
                                  Qt::QueuedConnection);
    }
}

bool TrajectoryService::isPaused() const
{
    return sendWorker_ != nullptr && sendWorker_->isPaused();
}

bool TrajectoryService::stopSendThread(int timeoutMs)
{
    if (sendThread_ == nullptr && sendWorker_ == nullptr) {
        return true;  // 没有线程在跑
    }

    // 先发取消信号
    cancelSendTrajectory();

    // 等待线程结束
    if (sendThread_ != nullptr) {
        sendThread_->quit();
        if (!sendThread_->wait(timeoutMs)) {
            // 超时：强制终止
            sendThread_->terminate();
            sendThread_->wait(1000);
            qDebug() << "TrajectoryService::stopSendThread: thread terminated after timeout";
        }
    }

    cleanupSendThread();
    return true;
}

void TrajectoryService::cleanupSendThread()
{
    sendWorker_ = nullptr;
    sendThread_ = nullptr;
    sending_ = false;
}
