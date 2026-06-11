#include "MotionService.h"

MotionService::MotionService(ZMotionDriver* driver,
                             TraceProtocol* traceProtocol,
                             QObject* parent)
    : QObject(parent)
    , driver_(driver)
    , jointProtocol_(driver)
    , traceProtocol_(traceProtocol)
{
}

// ── Joint 模式 ────────────────────────────────────────

Result MotionService::enterJointMode()
{
    if (driver_ == nullptr) {
        return Result::fail(3301, "ZMotionDriver 未初始化");
    }

    if (!driver_->isOpen()) {
        return Result::fail(3302, "控制器未连接");
    }

    return jointProtocol_.enterJointMode();
}

Result MotionService::exitJointMode()
{
    if (driver_ == nullptr) {
        return Result::fail(3303, "ZMotionDriver 未初始化");
    }

    return jointProtocol_.exitJointMode();
}

Result MotionService::sendDirectJoint(float j1, float j2, float j3,
                                      float j4, float j5, int speedLevel)
{
    if (driver_ == nullptr) {
        return Result::fail(3304, "ZMotionDriver 未初始化");
    }

    if (!driver_->isOpen()) {
        return Result::fail(3305, "控制器未连接");
    }

    float cmd[kJointCmdSize];
    cmd[0] = kCmdManualJoint;
    cmd[1] = j1;
    cmd[2] = j2;
    cmd[3] = j3;
    cmd[4] = j4;
    cmd[5] = j5;
    cmd[6] = static_cast<float>(speedLevel);

    return jointProtocol_.sendJointCommand(cmd);
}

// ── Trace 模式 ────────────────────────────────────────

Result MotionService::enterTraceMode()
{
    if (driver_ == nullptr) {
        return Result::fail(3405, "ZMotionDriver 未初始化");
    }

    if (!driver_->isOpen()) {
        return Result::fail(3406, "控制器未连接");
    }

    if (traceProtocol_ == nullptr) {
        return Result::fail(3408, "TraceProtocol 未初始化");
    }

    return traceProtocol_->enterTraceMode();
}

Result MotionService::canExitTraceMode()
{
    if (driver_ == nullptr) {
        return Result::fail(3407, "ZMotionDriver 未初始化");
    }

    if (traceProtocol_ == nullptr) {
        return Result::fail(3409, "TraceProtocol 未初始化");
    }

    return traceProtocol_->canExitTrace();
}
