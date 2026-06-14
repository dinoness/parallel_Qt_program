#include "CommandProtocol.h"

CommandProtocol::CommandProtocol(ZMotionDriver* driver)
    : driver_(driver)
{
}

Result CommandProtocol::sendEvent(int regAddr, int eventId)
{
    if (driver_ == nullptr) {
        return Result::fail(3801, "ZMotionDriver 未初始化");
    }

    if (!driver_->isOpen()) {
        return Result::fail(3802, "控制器未连接");
    }

    return driver_->writeModbusReg(regAddr, static_cast<uint16_t>(eventId));
}

Result CommandProtocol::sendEventLevel0(int eventId)
{
    return sendEvent(kRegEventLevel0, eventId);
}

Result CommandProtocol::sendEventLevel1(int eventId)
{
    return sendEvent(kRegEventLevel1, eventId);
}

Result CommandProtocol::sendEventLevel2(int eventId)
{
    return sendEvent(kRegEventLevel2, eventId);
}

Result CommandProtocol::sendHome()
{
    return sendEventLevel2(kEventHome);
}

Result CommandProtocol::sendPause()
{
    return sendEventLevel2(kEventPause);
}

Result CommandProtocol::sendResume()
{
    return sendEventLevel2(kEventResume);
}

Result CommandProtocol::sendStop()
{
    return sendEventLevel2(kEventStop);
}

Result CommandProtocol::sendEstop()
{
    return sendEventLevel0(kEventEstop);
}

Result CommandProtocol::sendErrorReset()
{
    return sendEventLevel0(kEventErrorReset);
}
