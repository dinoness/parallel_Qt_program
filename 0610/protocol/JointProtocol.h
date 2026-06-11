#pragma once

#include "../core/ProtocolConstants.h"
#include "../core/Result.h"
#include "../zmotion/ZMotionDriver.h"

/// @brief 关节指令协议 — 封装 Direct Joint 模式的 TABLE 写入与 MODBUS 寄存器操作
///
/// 环形缓冲: 5 条指令轮流写入 TABLE[300~334]。
/// 状态读回: MODBUS[80~89]。
/// 事件通知: MODBUS[92]。
class JointProtocol
{
public:
    explicit JointProtocol(ZMotionDriver* driver);

    /// @brief 进入关节模式 — 发送 EVENT_JOINT 到 MODBUS[92]
    Result enterJointMode();

    /// @brief 退出关节模式 — 发送 EVENT_JOINT_DONE 到 MODBUS[92]
    Result exitJointMode();

    /// @brief 下发一条关节指令到 TABLE 环形缓冲
    /// @param cmd 7 个 float: [cmd, j1, j2, j3, j4, j5, speed]
    Result sendJointCommand(const float cmd[kJointCmdSize]);

    /// @brief 读取当前写指针位置的指令状态
    Result readJointStatus(int slot, uint16& status);

private:
    /// @brief 事件寄存器写入前等待 — 轮询直到寄存器为 0 或超时 (5s)
    Result waitForEventReg(uint16& stuckValue);

    ZMotionDriver* driver_;
    int writeIndex_ = 0;  // 当前写入槽位 (0~4)
};
