#pragma once

#include <stdint.h>



// ===================================================================
// 共享寄存器 (所有模式共用)
// ===================================================================

constexpr int kEventReg         = 92;   // 事件寄存器
constexpr int kSystemStateReg   = 90;   // 系统状态寄存器

// ── 系统状态值 ───────────────────────────────────────
constexpr uint16_t kSysReady  = 4;
constexpr uint16_t kSysRunning  = 5;    // 控制器轨迹运行中

// ===================================================================
// Direct Joint (Manual Joint)
// ===================================================================

constexpr int kJointTableStart  = 300;   // TABLE 起始地址
constexpr int kJointCmdSize     = 7;     // 每条指令 7 个 float
constexpr int kJointBufferSize  = 5;     // 环形缓冲指令条数
constexpr int kJointTableSize   = kJointBufferSize * kJointCmdSize;  // 35
constexpr int kJointStatusBase  = 80;    // 指令执行状态 (80~89)

constexpr int kCmdManualJoint   = 1;     // 指令代号 (写入 TABLE 首元素)

// ===================================================================
// 事件 ID (写入 MODBUS kEventReg)
// ===================================================================
constexpr int kEventIdle        = 0;
constexpr int kEventHome        = 1;
constexpr int kEventJoint       = 3;  // 进入 Direct Joint
constexpr int kEventJointDone   = 4;  // 退出 Direct Joint
constexpr int kEventTraj        = 7;  // 进入 Trace
