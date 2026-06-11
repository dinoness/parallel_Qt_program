#pragma once

#include <QObject>
#include <QString>
#include <QThread>
#include <QVector>

#include "../core/Result.h"
#include "../motion/TrajectoryFile.h"
#include "../motion/TrajectoryTypes.h"
#include "../protocol/TraceProtocol.h"
#include "../zmotion/ZMotionDriver.h"
#include "../worker/TrajectorySendWorker.h"

class TrajectoryService : public QObject
{
    Q_OBJECT

public:
    explicit TrajectoryService(ZMotionDriver* driver,
                               TraceProtocol* protocol,
                               const QString& dataDir,
                               QObject* parent = nullptr);
    ~TrajectoryService();

    // ── 文件操作 ──────────────────────────────────────
    Result generateAndSave(const QString& fileName);
    Result loadPoints(const QString& fileName, QVector<TrajectoryPoint>& points);
    Result datToCsv(const QString& datFile, const QString& csvFile);
    Result csvToDat(const QString& csvFile, const QString& datFile);

    // ── 同步下发（调试用） ─────────────────────────────
    Result sendToControllerSync(const QString& fileName);

    // ── 异步下发（UI 调用） ────────────────────────────
    Result startSendTrajectoryAsync(const QString& filePath);
    Result startSendTrajectoryAsync(const QVector<TrajectoryPoint>& points);
    void   cancelSendTrajectory();
    bool   isSending() const;

signals:
    void sendProgressChanged(int sentGroups, int totalGroups, int percent);
    void sendFinished(Result result);
    void sendingStateChanged(bool sending);

private:
    void cleanupSendThread();

    ZMotionDriver*      driver_   = nullptr;
    TraceProtocol*      protocol_ = nullptr;
    TrajectoryFile      file_;

    QThread*             sendThread_ = nullptr;
    TrajectorySendWorker* sendWorker_ = nullptr;
    bool                 sending_    = false;
};
