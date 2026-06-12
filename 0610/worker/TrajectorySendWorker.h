#pragma once

#include <QObject>
#include <QVector>
#include <atomic>

#include "../core/Result.h"
#include "../motion/TrajectoryTypes.h"
#include "../protocol/TraceProtocol.h"

/// @brief 轨迹下发后台 Worker — 运行在独立 QThread 中
///
/// 负责阻塞式等待缓冲区、写入 TABLE、发事件、报告进度、响应取消。
/// 禁止直接访问 UI 控件。
class TrajectorySendWorker : public QObject
{
    Q_OBJECT

public:
    explicit TrajectorySendWorker(TraceProtocol* protocol,
                                  QObject* parent = nullptr);

public slots:
    void startSend(QVector<TrajectoryPoint> points);
    void cancel();

signals:
    void progressChanged(int sentGroups, int totalGroups, int percent);
    void finished(Result result);

private:
    TraceProtocol* protocol_ = nullptr;
    std::atomic_bool cancelRequested_;
};
