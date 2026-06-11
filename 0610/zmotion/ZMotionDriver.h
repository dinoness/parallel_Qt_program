#pragma once

#include <QString>
#include <QVector>
#include <QMutex>

#include "zaux.h"
#include "zmotion.h"

#include "../core/Result.h"



class ZMotionDriver
{
public:
    ZMotionDriver();
    ~ZMotionDriver();

    // 禁止拷贝，避免两个对象持有同一个 ZMC_HANDLE
    ZMotionDriver(const ZMotionDriver&) = delete;
    ZMotionDriver& operator=(const ZMotionDriver&) = delete;

    
    Result openEth(const QString& ip);
    Result closeEth();

    bool isOpen() const;

    // 后续扩展：寄存器、TABLE、在线命令等
    Result ipScan(QStringList& ipList);
    Result writeModbusReg(int addr, uint16 value);
    Result readModbusReg(int addr, uint16& value);

private:
    ZMC_HANDLE handle_;
    bool opened_;  // 记录是否已连接

    mutable QMutex mutex_;
};
