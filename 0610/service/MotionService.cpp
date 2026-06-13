#include "MotionService.h"

MotionService::MotionService(ZMotionDriver* driver,
                             TraceProtocol* traceProtocol,
                             QObject* parent)
    : QObject(parent)
    , driver_(driver)
    , jointProtocol_(driver)
    , cartJogProtocol_(driver)
    , traceProtocol_(traceProtocol)
{
}

// ── Home ─────────────────────────────────────────────

Result MotionService::sendHome()
{
    if (driver_ == nullptr) {
        return Result::fail(3501, "ZMotionDriver 未初始化");
    }

    if (!driver_->isOpen()) {
        return Result::fail(3502, "控制器未连接");
    }

    return driver_->writeModbusReg(kRegEventLevel2,
                                   static_cast<uint16>(kEventHome));
}

// ── 全局控制 ──────────────────────────────────────────

Result MotionService::sendPause()
{
    if (driver_ == nullptr) {
        return Result::fail(3701, "ZMotionDriver 未初始化");
    }

    if (!driver_->isOpen()) {
        return Result::fail(3702, "控制器未连接");
    }

    return driver_->writeModbusReg(kRegEventLevel2,
                                   static_cast<uint16>(kEventPause));
}

Result MotionService::sendResume()
{
    if (driver_ == nullptr) {
        return Result::fail(3703, "ZMotionDriver 未初始化");
    }

    if (!driver_->isOpen()) {
        return Result::fail(3704, "控制器未连接");
    }

    return driver_->writeModbusReg(kRegEventLevel2,
                                   static_cast<uint16>(kEventResume));
}

Result MotionService::sendStop()
{
    if (driver_ == nullptr) {
        return Result::fail(3705, "ZMotionDriver 未初始化");
    }

    if (!driver_->isOpen()) {
        return Result::fail(3706, "控制器未连接");
    }

    return driver_->writeModbusReg(kRegEventLevel2,
                                   static_cast<uint16>(kEventStop));
}

Result MotionService::sendEstop()
{
    if (driver_ == nullptr) {
        return Result::fail(3707, "ZMotionDriver 未初始化");
    }

    if (!driver_->isOpen()) {
        return Result::fail(3708, "控制器未连接");
    }

    return driver_->writeModbusReg(kRegEventLevel0,
                                   static_cast<uint16>(kEventEstop));
}

Result MotionService::sendErrorReset()
{
    if (driver_ == nullptr) {
        return Result::fail(3709, "ZMotionDriver 未初始化");
    }

    if (!driver_->isOpen()) {
        return Result::fail(3710, "控制器未连接");
    }

    return driver_->writeModbusReg(kRegEventLevel0,
                                   static_cast<uint16>(kEventErrorReset));
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
    cmd[0] = kCmdMove;
    cmd[1] = j1;
    cmd[2] = j2;
    cmd[3] = j3;
    cmd[4] = j4;
    cmd[5] = j5;
    cmd[6] = static_cast<float>(speedLevel);

    return jointProtocol_.sendJointCommand(cmd);
}

// ── Cart Jog 模式 ─────────────────────────────────────

Result MotionService::enterCartJogMode()
{
    if (driver_ == nullptr) {
        return Result::fail(3601, "ZMotionDriver 未初始化");
    }

    if (!driver_->isOpen()) {
        return Result::fail(3602, "控制器未连接");
    }

    return cartJogProtocol_.enterCartJogMode();
}

Result MotionService::exitCartJogMode()
{
    if (driver_ == nullptr) {
        return Result::fail(3603, "ZMotionDriver 未初始化");
    }

    return cartJogProtocol_.exitCartJogMode();
}

Result MotionService::sendCartJog(int cmdId,
                                  float x, float y, float z,
                                  float phi, float theta, int speedLevel)
{
    if (driver_ == nullptr) {
        return Result::fail(3604, "ZMotionDriver 未初始化");
    }

    if (!driver_->isOpen()) {
        return Result::fail(3605, "控制器未连接");
    }

    float cmd[kCartJogCmdSize];
    cmd[0] = static_cast<float>(cmdId);
    cmd[1] = x;
    cmd[2] = y;
    cmd[3] = z;
    cmd[4] = phi;
    cmd[5] = theta;
    cmd[6] = static_cast<float>(speedLevel);

    return cartJogProtocol_.sendCartJogCommand(cmd);
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

Result MotionService::exitTraceMode()
{
    if (driver_ == nullptr) {
        return Result::fail(3415, "ZMotionDriver 未初始化");
    }

    if (traceProtocol_ == nullptr) {
        return Result::fail(3416, "TraceProtocol 未初始化");
    }

    return traceProtocol_->exitTraceMode();
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
