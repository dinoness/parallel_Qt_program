#include "CommandWriter.h"

CommandWriter::CommandWriter(ZMotionDriver* driver)
    : driver_(driver)
{
}

Result CommandWriter::sendEvent(int eventId)
{
    if (driver_ == nullptr) {
        return Result::fail(3101, "ZMotionDriver 未初始化");
    }

    return driver_->writeModbusReg(Reg::EVENT_NORMAL, static_cast<uint16>(eventId));
}
