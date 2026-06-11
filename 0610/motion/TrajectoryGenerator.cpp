#include "TrajectoryGenerator.h"
#include <math.h>

#define PI 3.14159265358979323846

QVector<TrajectoryPoint> TrajectoryGenerator::generateScrew()
{
    QVector<TrajectoryPoint> points;

    float n_ticks = 50;
    float z_height = -600.0f * 1000.0f;  // um
    float R0 = 30.0f * 1000.0f;          // um

    {
        TrajectoryPoint p;
        p.cmd = 2;
        p.x = 0;
        p.y = 0;
        p.z = z_height;
        p.theta = 0;
        p.phi = 0;
        p.ticks = n_ticks;
        points.append(p);
    }

    float time_seg = n_ticks / 1000.0f;
    float vR = R0 / 60.0f;
    float point_num = 60.0f * 1000.0f / n_ticks;
    float omega = PI / 10.0f;

    for (int i = 1; i <= static_cast<int>(point_num); i++) {
        TrajectoryPoint p;
        p.cmd = 10;
        float t = i * time_seg;
        float r = vR * t;
        p.x = r * sin(omega * t);
        p.y = r * cos(omega * t);
        p.z = z_height;
        p.theta = 0;
        p.phi = 0;
        p.ticks = n_ticks;
        points.append(p);
    }

    {
        TrajectoryPoint p;
        p.cmd = 2;
        p.x = 0;
        p.y = R0;
        p.z = z_height;
        p.theta = 0;
        p.phi = 0;
        p.ticks = 0;
        points.append(p);
    }

    return points;
}
