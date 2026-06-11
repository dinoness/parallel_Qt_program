#pragma once

#include <stdint.h>

constexpr int kCmdSize = 7;          // [cmd, x, y, z, theta, phi, ticks]
constexpr int kDataGroupSize = 100;  // 每组数据点数
constexpr int kDataGroupNum = 10;    // 缓冲块数量
constexpr int kDataBlockSize = kDataGroupSize * kCmdSize;
constexpr int kDataStartIndex = 1000;
constexpr uint16_t kDataUpdate = 1;
constexpr uint16_t kDataUsed = 2;
constexpr uint16_t kDataBlank = 3;

struct TrajectoryPoint {
    float cmd;
    float x;
    float y;
    float z;
    float theta;
    float phi;
    float ticks;

    TrajectoryPoint()
        : cmd(0), x(0), y(0), z(0), theta(0), phi(0), ticks(0) {}

    void toArray(float arr[kCmdSize]) const {
        arr[0] = cmd;
        arr[1] = x;
        arr[2] = y;
        arr[3] = z;
        arr[4] = theta;
        arr[5] = phi;
        arr[6] = ticks;
    }

    static TrajectoryPoint fromArray(const float arr[kCmdSize]) {
        TrajectoryPoint p;
        p.cmd = arr[0];
        p.x = arr[1];
        p.y = arr[2];
        p.z = arr[3];
        p.theta = arr[4];
        p.phi = arr[5];
        p.ticks = arr[6];
        return p;
    }
};
