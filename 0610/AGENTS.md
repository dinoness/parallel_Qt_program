# Parallel Robot Host — 上位机程序

## 项目概述

并联机器人控制器的 Qt/C++ 上位机程序。通过以太网与 ZMotion 控制器通信，提供轨迹生成、文件转换、轨迹下发、手动关节控制、回零等功能。

- **语言**: C++11
- **UI 框架**: Qt 5.15.2 (Widgets)
- **构建系统**: qmake (1202.pro)
- **编译器**: MinGW 64-bit (Qt 5.15.2 套件)
- **控制器通信**: ZMotion ZAux DLL（`zaux.h` / `zmotion.h`）
- **第三方库**: QXlsx（Excel 读写，已集成源码）

## 目录结构

```
0610/                           ← 项目根目录
├─ 1202.pro                    ← qmake 工程文件
├─ main.cpp                    ← 入口：注册 MetaType，创建 AppContext + MainWindow
├─ mainwindow.h / .cpp / .ui   ← UI 层：按钮槽函数、状态栏消息
│
├─ app/
│  └─ AppContext.h / .cpp      ← 依赖注入容器：持有所有 Service / Driver / Protocol 实例
│
├─ core/
│  └─ Result.h                 ← 通用返回值：{bool ok, int code, QString message}，已注册 QMetaType
│
├─ zmotion/
│  └─ ZMotionDriver.h / .cpp   ← ZAux DLL 的最薄封装：openEth / closeEth / trigger / setTable / readModbusReg / writeModbusReg / ipScan，所有方法用 QMutex 保护
│
├─ protocol/
│  ├─ ControllerProtocol.h/.cpp ← 协议总入口：聚合 TableBufferWriter + CommandWriter，提供 sendTrajectory()
│  ├─ TableBufferWriter.h/.cpp  ← TABLE 缓冲握手协议：等待 MODBUS 状态 → 写 TABLE → 标记 Update，支持超时/取消/进度回调
│  └─ CommandWriter.h/.cpp      ← 在线命令写入：sendEvent() 写 MODBUS 事件寄存器 (地址 100)
│
├─ service/
│  ├─ ConnectionService.h/.cpp  ← 连接管理：connectToController / disconnectFromController，发送 connectionChanged 信号
│  └─ TrajectoryService.h/.cpp  ← 轨迹业务：生成 / 文件转换 / 同步下发 / 异步下发（管理 QThread + Worker 生命周期）
│
├─ motion/
│  ├─ TrajectoryTypes.h         ← 公共类型：TrajectoryPoint (7 float)，协议常量 (kCmdSize=7, kDataGroupSize=100 等)
│  ├─ TrajectoryFile.h/.cpp     ← 文件 I/O：二进制 .dat 读写 (QDataStream, LE, SinglePrecision)，CSV 读写
│  └─ TrajectoryGenerator.h/.cpp ← 轨迹生成：generateScrew() 螺旋线轨迹
│
├─ worker/
│  └─ TrajectorySendWorker.h/.cpp ← 后台下发 Worker：运行在独立 QThread，调用 ControllerProtocol，支持取消 (atomic_bool)
│
├─ robot_trace.h / .cpp        ← 【已废弃】旧版轨迹线程类，不再编译进工程，待删除
└─ architecture.md             ← 架构说明文档
```

## 分层架构

```
┌──────────────┐
│   UI 层       │  MainWindow: 按钮 → 调用 Service，信号 → 更新状态栏
├──────────────┤
│ Service 层    │  TrajectoryService / ConnectionService: 业务编排，管理线程生命周期
├──────────────┤
│ Protocol 层   │  ControllerProtocol → TableBufferWriter / CommandWriter: 寄存器/TABLE 地址封装
├──────────────┤
│ Driver 层     │  ZMotionDriver: ZAux 函数薄封装 + QMutex 线程安全
├──────────────┤
│ 硬件层        │  ZMotion 控制器 (以太网)
└──────────────┘
```

**规则**:
- UI 层不直接调用 ZAux，不直接创建 QThread
- Service 层不直接操作寄存器地址（委托给 Protocol 层）
- Protocol 层不直接调用 ZAux（通过 ZMotionDriver）
- ZMotionDriver 是唯一持有 `ZMC_HANDLE` 的类，所有 ZAux 调用经过它
- Worker 线程禁止访问 UI 控件、禁止弹 QMessageBox

## 构建与运行

### 构建

```bash
# 在 Qt Creator 中打开 1202.pro，选择 Desktop Qt 5.15.2 MinGW 64-bit 套件
# 或命令行：
qmake 1202.pro
mingw32-make -j4
```

构建产物位于 `../1202/build/Desktop_Qt_5_15_2_MinGW_64_bit-Debug/debug/1202.exe`。

### 运行时依赖

- `third_party/zaux/zauxdll.dll` — ZMotion 动态库
- `../../trace_data/` — 轨迹数据目录（相对 exe 路径，对应 `0610/trace_data/`）

## 数据格式

### TrajectoryPoint（7 个 float）

```
[cmd, x, y, z, theta, phi, ticks]
  0    1  2  3   4      5     6
```

- `cmd=2`: MOVEABS（绝对直线）
- `cmd=10`: MOVE_PTABS（单位时间距离绝对）
- `cmd=1`: MOVE(0)（空指令，用于填充）
- 坐标单位: um (微米)
- `ticks`: 时间片数（1 tick = 1ms 伺服周期）

### .dat 文件

二进制格式，QDataStream LittleEndian SinglePrecision，每 7 个 float 一组。

### .csv 文件

文本格式，逗号分隔。表头: `cmd,x,y,z,theta,phi,ticks`

## TABLE 缓冲协议

轨迹下发使用 10 组环形 TABLE 缓冲：

| 参数 | 值 | 说明 |
|------|-----|------|
| 缓冲组数 | 10 | 环形轮流写入 |
| 每组指令数 | 100 | kDataGroupSize |
| 每组 float 数 | 700 | 100 × 7 (kDataBlockSize) |
| TABLE 起始地址 | 1000 | kDataStartIndex |
| MODBUS 状态寄存器 | 0~9 | 对应 10 个缓冲组 |
| kDataUpdate=1 | 已更新 | 等待控制器消费 |
| kDataUsed=2 | 已使用 | 控制器已取走 |
| kDataBlank=3 | 空闲 | 可写入 |

### 下发流程 (sendToController)

1. `trigger()` — 触发控制器
2. 循环：取当前 groupId = loopNum % 10
3. `waitBufferReady(groupId)` — 轮询 MODBUS 直到 state ≠ Update，支持超时 (60s) 和取消
4. 打包 blockData[700]：从 TrajectoryPoint 取出 100 组 × 7 float，末尾不足补 MOVE(0)
5. `setTable(1000 + groupId*700, 700, blockData)` — 写入 TABLE
6. `writeModbusReg(groupId, kDataUpdate)` — 标记已更新
7. 进度回调 `onProgress(loopNum+1, totalGroups)`
8. 全部发送完后 `sendEvent(TrajPrepare)` — 写 MODBUS 寄存器 100

## 线程模型

### 异步下发（当前实现）

```
UI 线程                            Worker 线程
  │                                   │
  ├─ TrajectoryService::              │
  │   startSendTrajectoryAsync()      │
  │   ├─ 读文件                       │
  │   ├─ 创建 QThread +               │
  │   │   TrajectorySendWorker        │
  │   └─ sendThread->start() ────→   ├─ startSend(points)
  │                                   │   └─ ControllerProtocol::
  │  ← progressChanged ────────────   │       sendTrajectory(points, options)
  │  ← sendFinished ───────────────   │       ├─ TableBufferWriter::sendAll()
  │                                   │       └─ CommandWriter::sendEvent()
  ├─ cancelSendTrajectory() ────→    ├─ cancelRequested_.store(true)
  └─ ~TrajectoryService()            └─ 安全退出
       cancel → quit → wait(3000)
```

### 同步下发（仅调试用）

`TrajectoryService::sendToControllerSync()` 在当前线程阻塞执行，UI 不应调用。

## 新增功能的开发规范

### 添加新 Service

1. 在 `service/` 下创建类，构造函数接受 `ZMotionDriver*`
2. 在 `AppContext` 中声明成员并初始化（成员声明顺序注意依赖关系）
3. 在 `1202.pro` 的 SOURCES/HEADERS 中添加

### 添加新 Protocol

1. 在 `protocol/` 下创建类，构造函数接受 `ZMotionDriver*`
2. 寄存器地址/TABLE 偏移等常量封装在 `.cpp` 中，不作为公开 API
3. 对外暴露语义化方法（如 `sendEvent()` 而非 `writeModbusReg(100, val)`）

### 添加新 Worker

1. 在 `worker/` 下创建 QObject 子类
2. 用 `std::atomic_bool` 支持取消
3. 通过 queued signal 报告进度和结果
4. 禁止 include UI 头文件

### 跨线程类型

以下类型已在 `main.cpp` 注册可用于 queued signal：
- `Result` (Q_DECLARE_METATYPE in core/Result.h)
- `TrajectoryPoint` (Q_DECLARE_METATYPE in motion/TrajectoryTypes.h)
- `QVector<TrajectoryPoint>`

## AppContext 成员声明顺序

成员按依赖顺序声明（构造顺序依赖）：

```cpp
ZMotionDriver driver_;                    // 1. 最底层，无依赖
ControllerProtocol protocol_;             // 2. 依赖 driver_
ConnectionService connectionService_;     // 3. 依赖 driver_
TrajectoryService trajectoryService_;     // 4. 依赖 driver_ + protocol_
```

## UI 按钮映射

| 按钮 objectName | 文本 | 功能 |
|---|---|---|
| btn_ip_scan | IP SCAN | 扫描局域网控制器 |
| btn_connect_controller | Connect | 连接控制器 |
| btn_disconnect_controller | Disconnect | 断开控制器 |
| pushButton | Home | 回零 |
| btn_direct_joint_enter/send/exit | ENTER/SEND/EXIT | 直接关节控制（未实现） |
| btn_jog_enter/send/exit | ENTER/SEND/EXIT | 笛卡尔 Jog（未实现） |
| btn_trace_enter | ENTER | 进入轨迹模式（未实现） |
| btn_jog_exit_2 | EXIT | 退出轨迹模式（未实现） |
| btn_trace_send | SEND | 轨迹下发（未来） |
| btn_thread_open | Thread Open | **取消下发**（复用按钮） |
| btn_thread_close | Thread Close | 空操作 |
| btn_trace_to_dat | Trace to Dat | 生成 .dat 轨迹 |
| btn_trace_read | Trace Read | 空操作 |
| btn_trace_to_xlsx | Dat to Csv | .dat → .csv |
| btn_xlsx_to_dat | Csv to Dat | .csv → .dat |

## ZMotionDriver API

```cpp
Result openEth(const QString& ip);
Result closeEth();
bool isOpen() const;
Result ipScan(QStringList& ipList);
Result trigger();
Result setTable(int start, int count, const float* data);
Result writeModbusReg(int addr, uint16 value);
Result readModbusReg(int addr, uint16& value);
```

所有方法内部使用 `QMutexLocker`，支持多线程安全调用。

## 已废弃代码

- `robot_trace.h/.cpp` — 旧版轨迹线程（QThread + 信号/槽 + QXlsx），已被 `motion/` + `service/` + `worker/` 替代。文件仍在目录中但不再编译。
- `mainwindow.cpp` 中的 `trace_test()`, `up_State()`, `commandCheckHandler()`, `thread_open()`, `thread_close()` — 已移除。
- QXlsx 库 (`QXlsx/`) — 仍在工程中，但轨迹功能已改用 CSV，仅保留以备他用。
