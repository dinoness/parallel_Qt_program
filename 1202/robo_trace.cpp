#include "robo_trace.h"
#define PI 3.14159

robo_trace::robo_trace()
{

}


void robo_trace::run()
{

}


bool robo_trace::trace_to_file1(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QDataStream out(&file);
    // 关键：设置字节序为小端，保证跨平台一致性（也可用QDataStream::BigEndian）
    out.setByteOrder(QDataStream::LittleEndian);
    // 关键：设置双精度浮点数精度为标准32位
    out.setFloatingPointPrecision(QDataStream::SinglePrecision);

    /*  reference code
    for (const auto& group : dataGroups) {
        if (group.size() != 6) {
            // 处理错误：组大小不符
            continue;
        }
        for (double value : group) {
            out << value; // 流式写入，自动处理二进制编码
        }
    }*/

    int i;
    float u_data[6];
    for(i = 0; i < 360; i++)
    {

        u_data[0] = (sin(((2*PI)/360)*i))*20;
        u_data[1] = (cos(((2*PI)/360)*i))*20;
        u_data[2] = 0;
        u_data[3] = 0;
        u_data[4] = 0;
        u_data[5] = 0;


        for (float value : u_data) {
            out << value; // 流式写入，自动处理二进制编码
        }

    }

    file.close();

    return true;
}
