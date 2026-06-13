#pragma once

#include <stdint.h>

// ===================================================================
// 共享常量
// ===================================================================

constexpr int kCmdSize = 7;  // 每条指令 7 个 float: [cmd, ...]

// ── 数据状态 ─────────────────────────────────────────
constexpr uint16_t kDataUpdate = 1;  // 已更新，等待控制器消费
constexpr uint16_t kDataUsed   = 2;  // 已使用，控制器已取走
constexpr uint16_t kDataBlank  = 3;  // 空闲，可写入

// ===================================================================
// MODBUS REG寄存器分配
// ===================================================================
constexpr int kRegSystemState         = 5;
constexpr int kRegMotionMode          = 6;
constexpr int kRegActiveTask          = 8;
constexpr int kRegEventLevel0         = 90;
constexpr int kRegEventLevel1         = 91;
constexpr int kRegEventLevel2         = 92;   // 事件寄存器

// ===================================================================
// 系统状态值
// ===================================================================
constexpr uint16_t kSysServoReady = 2;
constexpr uint16_t kSysHoming = 3;
constexpr uint16_t kSysReady  = 4;
constexpr uint16_t kSysRunning  = 5;    // 控制器轨迹运行中
constexpr uint16_t kSysPaused = 6;
constexpr uint16_t kSysError = 8;
constexpr uint16_t kSysEstop = 9;
// ===================================================================
// Direct Joint (Manual Joint)
// ===================================================================

constexpr int kJointTableStart  = 300;   // TABLE 起始地址
constexpr int kJointCmdSize     = 7;     // 每条指令 7 个 float
constexpr int kJointBufferSize  = 5;     // 环形缓冲指令条数
constexpr int kJointTableSize   = kJointBufferSize * kJointCmdSize;  // 35
constexpr int kRegJointStatusBase  = 80;    // 指令执行状态REG地址 (80~89)

// ===================================================================
// Cart Jog
// ===================================================================

constexpr int kCartJogTableStart  = 350;   // TABLE 起始地址
constexpr int kCartJogCmdSize     = 7;     // 每条指令 7 个 float
constexpr int kCartJogBufferSize  = 5;     // 环形缓冲指令条数
constexpr int kCartJogTableSize   = kCartJogBufferSize * kCartJogCmdSize;  // 35
constexpr int kRegCartJogStatusBase  = 70;    // 指令执行状态REG地址 (70~79)

// ===================================================================
// Traj
// ===================================================================

constexpr int kTrajTableStart  = 1000;   // TABLE 起始地址
constexpr int kTrajCmdSize     = 7;      // 每条指令 7 个 float
constexpr int kTrajGroupSize   = 100;    // 每组指令数
constexpr int kTrajGroupNum    = 10;     // 环形缓冲组数
constexpr int kTrajBlockSize   = kTrajGroupSize * kTrajCmdSize;  // 700 = 每组 TABLE 大小
constexpr int kRegTrajStatusBase  = 50;     // 指令执行状态REG地址 (50~69)

// ===================================================================
// 事件 ID (写入 MODBUS kRegEventLevel2 = 92)
// ===================================================================

constexpr int kEventIdle        = 0;
constexpr int kEventHome        = 1;
constexpr int kEventJoint       = 3;  // 进入 Direct Joint
constexpr int kEventJointDone   = 4;  // 退出 Direct Joint
constexpr int kEventCartJog     = 5;
constexpr int kEventCartJogDone = 6;
constexpr int kEventTraj        = 7;  // 进入 Trace
constexpr int kEventStop        = 9;
constexpr int kEventPause       = 10;
constexpr int kEventResume      = 11;
constexpr int kEventErrorReset  = 90;
constexpr int kEventEstop       = 99;

// ===================================================================
// 运动指令ID
// ===================================================================

constexpr int kCmdNone   = 0;
constexpr int kCmdMove   = 1;
constexpr int kCmdMoveAbs = 2;
constexpr int kCmdPause  = 3;
constexpr int kCmdResume = 4;
constexpr int kCmdStop   = 5;
constexpr int kCmdEstop  = 99;
