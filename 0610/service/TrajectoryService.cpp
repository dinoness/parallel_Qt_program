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

// ── 同步下发（调试用，UI 不应直接调用） ──────────────────────────

Result TrajectoryService::sendToControllerSync(const QString& fileName)
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

    qDebug() << "sendToControllerSync: read" << points.size() << "points";

    if (protocol_ == nullptr) {
        return Result::fail(3301, "TraceProtocol 未初始化");
    }

    return protocol_->sendTrajectory(points);
}

// ── 异步下发 ──────────────────────────────────────────────────────

Result TrajectoryService::startSendTrajectoryAsync(const QString& filePath)
{
    QVector<TrajectoryPoint> points;
    Result ret = loadPoints(filePath, points);
    if (!ret.ok) return ret;

    return startSendTrajectoryAsync(points);
}

Result TrajectoryService::startSendTrajectoryAsync(const QVector<TrajectoryPoint>& points)
{
    if (protocol_ == nullptr) {
        return Result::fail(3301, "TraceProtocol 未初始化");
    }

    if (points.isEmpty()) {
        return Result::fail(3302, "轨迹数据为空，无法下发");
    }

    if (sending_ || sendThread_ != nullptr || sendWorker_ != nullptr) {
        return Result::fail(3303, "当前已有轨迹正在下发");
    }

    sending_ = true;
    emit sendingStateChanged(true);

    sendThread_ = new QThread();
    sendWorker_ = new TrajectorySendWorker(protocol_);

    sendWorker_->moveToThread(sendThread_);

    connect(sendThread_, &QThread::started,
            sendWorker_, [this, points]() {
                sendWorker_->startSend(points);
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

void TrajectoryService::cleanupSendThread()
{
    sendWorker_ = nullptr;
    sendThread_ = nullptr;
    sending_ = false;
}
