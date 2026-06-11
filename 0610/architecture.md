ParallelRobotHost/
├─ main.cpp
├─ MainWindow.h
├─ MainWindow.cpp
├─ MainWindow.ui
│
├─ app/
│  ├─ AppContext.h
│  └─ AppContext.cpp
│
├─ core/
│  ├─ RobotTypes.h
│  ├─ Result.h
│  ├─ ErrorCode.h
│  ├─ CommandId.h
│  ├─ SystemState.h
│  └─ RegisterMap.h
│
├─ zmotion/ 封装库接口
│  ├─ ZMotionDriver.h
│  ├─ ZMotionDriver.cpp
│  ├─ ZMotionException.h
│  └─ ZMotionTypes.h
│
├─ protocol/
│  ├─ ControllerProtocol.h
│  ├─ ControllerProtocol.cpp
│  ├─ CommandWriter.h
│  ├─ CommandWriter.cpp
│  ├─ StatusReader.h
│  ├─ StatusReader.cpp
│  ├─ TableBufferWriter.h
│  ├─ TableBufferWriter.cpp
│  ├─ Heartbeat.h
│  └─ Heartbeat.cpp
│
├─ service/
│  ├─ ConnectionService.h/.cpp
│  ├─ HomeService.h/.cpp
│  ├─ ManualJointService.h/.cpp
│  ├─ JogService.h/.cpp
│  ├─ TrajectoryService.h/.cpp
│  ├─ MonitorService.h/.cpp
│  ├─ SafetyService.h/.cpp
│  └─ ParameterService.h/.cpp
│
├─ motion/
│  ├─ Trajectory.h
│  ├─ Trajectory.cpp
│  ├─ TrajectoryGenerator.h/.cpp
│  ├─ TrajectoryValidator.h/.cpp
│  ├─ TrajectoryFile.h/.cpp
│  └─ RobotKinematics.h/.cpp
│
├─ worker/
│  ├─ MonitorWorker.h/.cpp
│  ├─ TrajectorySendWorker.h/.cpp
│  └─ DataRecordWorker.h/.cpp
│
├─ config/
│  ├─ robot_config.json
│  ├─ axis_config.json
│  └─ controller_map.json
│
└─ third_party/
   └─ zaux/
      ├─ zauxdll.h
      ├─ zauxdll.lib
      ├─ zauxdll.dll
      └─ zmotion.dll



UI 层
 ↓
Service 层：HomeService / TrajectoryService / ManualJointService
 ↓
Protocol 层：CommandWriter / StatusReader / TableBufferWriter
 ↓
ZMotionDriver：对 ZAux 函数的最薄封装
 ↓
控制器 v2：main.bas + FSM + 各任务

#### UI层
显示按钮等UI界面；
收集用于输入；
并转化成把用户操作转成 Service 调用；
显示 Service 返回结果；

> 主要文件
    ui/MainWindow.h/.cpp/.ui

#### Service层
业务逻辑层，它不关心界面怎么画，也不直接调用 ZAux。它负责把一个完整功能组织起来

> 主要文件
    service/ConnectionService.h/.cpp
    service/HomeService.h/.cpp
    service/TrajectoryService.h/.cpp

#### Protocol层
协议层，负责写哪些寄存器，哪些TABLE，并将这些操作封装，由service层调用

> 主要文件
    core/RegisterMap.h
    core/CommandId.h
    protocol/CommandWriter.h/.cpp

#### ZMotionDriver层
封装ZMotion库，改成C++接口


#### core
公共数据类型和常量

#### motion
轨迹、运动学、文件处理

#### worker
所以周期监控、轨迹下发、数据记录要放到 worker



