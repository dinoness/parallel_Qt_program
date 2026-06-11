#include "ControllerProtocol.h"

ControllerProtocol::ControllerProtocol(ZMotionDriver* driver)
    : tableWriter_(driver),
      commandWriter_(driver)
{
}

TableBufferWriter* ControllerProtocol::tableWriter()
{
    return &tableWriter_;
}

CommandWriter* ControllerProtocol::commandWriter()
{
    return &commandWriter_;
}

Result ControllerProtocol::sendTrajectory(const QVector<TrajectoryPoint>& points,
                                           const TrajectorySendOptions& options)
{
    Result ret = tableWriter_.sendAll(points, options);
    if (!ret.ok) return ret;

    ret = commandWriter_.sendEvent(EventId::TrajPrepare);
    if (!ret.ok) return ret;

    return Result::success();
}
