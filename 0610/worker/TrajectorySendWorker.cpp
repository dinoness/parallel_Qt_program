#include "TrajectorySendWorker.h"

#include <QtGlobal>

TrajectorySendWorker::TrajectorySendWorker(TraceProtocol* protocol,
                                           QObject* parent)
    : QObject(parent),
      protocol_(protocol),
      cancelRequested_(false)
{
}

void TrajectorySendWorker::cancel()
{
    cancelRequested_.store(true);
}

void TrajectorySendWorker::startSend(QVector<TrajectoryPoint> points)
{
    cancelRequested_.store(false);

    if (protocol_ == nullptr) {
        emit finished(Result::fail(3201, "TraceProtocol 未初始化"));
        return;
    }

    TrajectorySendOptions options;
    options.timeoutMs = 10000;

    options.isCancelled = [this]() {
        return cancelRequested_.load();
    };

    options.onProgress = [this](int sentGroups, int totalGroups) {
        int percent = 0;
        if (totalGroups > 0) {
            percent = qBound(0, sentGroups * 100 / totalGroups, 100);
        }

        emit progressChanged(sentGroups, totalGroups, percent);
    };

    Result ret = protocol_->sendTrajectory(points, options);

    emit finished(ret);
}
