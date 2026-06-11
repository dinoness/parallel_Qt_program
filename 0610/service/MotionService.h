#pragma once

#include <QObject>

#include "../core/ProtocolConstants.h"
#include "../core/Result.h"
#include "../protocol/JointProtocol.h"
#include "../protocol/TraceProtocol.h"
#include "../zmotion/ZMotionDriver.h"

/// @brief 运动控制 Service — 协调 Direct Joint / Cart Jog / Home / Trace 等运动模式
class MotionService : public QObject
{
    Q_OBJECT

public:
    explicit MotionService(ZMotionDriver* driver,
                           TraceProtocol* traceProtocol,
                           QObject* parent = nullptr);

    // ── Joint 模式 ────────────────────────────────────
    Result enterJointMode();
    Result exitJointMode();
    Result sendDirectJoint(float j1, float j2, float j3, float j4, float j5,
                           int speedLevel);

    // ── Trace 模式 ────────────────────────────────────
    Result enterTraceMode();
    Result canExitTraceMode();

private:
    ZMotionDriver* driver_;
    JointProtocol  jointProtocol_;
    TraceProtocol* traceProtocol_;
};
