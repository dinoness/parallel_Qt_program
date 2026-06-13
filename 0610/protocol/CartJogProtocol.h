#pragma once

#include "../core/ProtocolConstants.h"
#include "../core/Result.h"
#include "../zmotion/ZMotionDriver.h"

/// @brief 笛卡尔 Jog 协议 — 封装 Cart Jog 模式的 TABLE 写入与 MODBUS 寄存器操作
///
/// 环形缓冲: 5 条指令轮流写入 TABLE[350~384]。
/// 状态读回: MODBUS[70~79]。
class CartJogProtocol
{
public:
    explicit CartJogProtocol(ZMotionDriver* driver);

    /// @brief 进入 Cart Jog 模式 — 初始化写指针，不下发事件
    Result enterCartJogMode();

    /// @brief 退出 Cart Jog 模式 — 清理写指针，不下发事件
    Result exitCartJogMode();

    /// @brief 下发一条 Cart Jog 指令到 TABLE 环形缓冲
    /// @param cmd 7 个 float: [cmd, x, y, z, phi, theta, speed]
    Result sendCartJogCommand(const float cmd[kCartJogCmdSize]);

    /// @brief 读取指定槽位的指令状态
    Result readCartJogStatus(int slot, uint16& status);

private:
    /// @brief 事件寄存器写入前等待 — 轮询直到寄存器为 0 或超时 (5s)
    Result waitForEventReg(uint16& stuckValue);

    /// @brief 等待槽位空闲 — 轮询直到状态 ≠ kDataUpdate 或超时
    Result waitSlotReady(int slot);

    ZMotionDriver* driver_;
    int writeIndex_ = 0;  // 当前写入槽位 (0~4)
};
