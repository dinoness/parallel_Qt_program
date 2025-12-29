#include "robot_trace.h"
#include <QApplication>

#define PI 3.14159265358979323846
#define F_DataUpdate (uint16)1
#define F_DataUsed (uint16)2
#define F_DataBlank (uint16)3
#define DataStart_Index 1000
#define DataGroupSize 100  // 数据块中的数据组数
#define CmdSize 7  // 每组数据大小，[指令代码,x,y,z,theta,phi,ticks(time = ticks * servo_period)]
#define DataGroupNum 10  // 数据块缓冲的块数
#define DataBlockSize  DataGroupSize*CmdSize  // 每次发送的数据总数

// command mapping
//  1 MOVE走直线
//  2 MOVEABS走直线绝对
// 10 MOVE_PTABS单位时间距离绝对


robot_trace::robot_trace()
{
    data_dir = "../../trace_data/";
}


bool robot_trace::trace_to_dat_test(const QString& cus_file_name)
{
    QString file_name = cus_file_name.isEmpty() ? "01" : cus_file_name;
    QString file_path = QString("%1%2.dat").arg(data_dir, file_name);


    QFile data_file(file_path);


    if (!data_file.open(QIODevice::WriteOnly)) {  // 注意写的时候判定是Write
        qWarning() << "无法打开文件：" << file_path;
        qWarning() << "错误信息：" << data_file.errorString();
        return false;
    }

    QDataStream out(&data_file);
    // 设置字节序为小端，保证跨平台一致性（也可用QDataStream::BigEndian）
    out.setByteOrder(QDataStream::LittleEndian);
    // 设置双精度浮点数精度为标准32位
    out.setFloatingPointPrecision(QDataStream::SinglePrecision);

    int i;
    float u_data[CmdSize];
    // sin-shape 1
    // for(i = 0; i < 360; i++)
    // {
    //     u_data[0] = 10;
    //     u_data[1] = (sin(((2*PI)/360)*i))*20;
    //     u_data[2] = (cos(((2*PI)/360)*i))*20;
    //     u_data[3] = 0;
    //     u_data[4] = 0;
    //     u_data[5] = 0;
    //     u_data[6] = 100;  // ticks


    //     for (float value : u_data) {
    //         out << value; // 流式写入，自动处理二进制编码
    //     }

    // }

    // 2 axis,line move

    u_data[0] = 2;
    u_data[1] = 20;  // mm
    u_data[2] = 20;  // mm
    u_data[3] = 0;
    u_data[4] = 0;
    u_data[5] = 0;
    u_data[6] = 0;  // ticks

    for (float value : u_data) {
        out << value; // 流式写入，自动处理二进制编码
    }

    for(i = 0; i < 100; i++)
    {
        u_data[0] = 10;
        u_data[1] = (i + 1) * 1;  // mm
        u_data[2] = (i + 1) * 1;  // mm
        u_data[3] = 0;
        u_data[4] = 0;
        u_data[5] = 0;
        u_data[6] = 100;  // ticks

        for (float value : u_data) {
            out << value; // 流式写入，自动处理二进制编码
        }
    }
    for(i = 0; i < 100; i++)
    {
        u_data[0] = 10;
        u_data[1] = 100 - (i + 1) * 1;  // mm
        u_data[2] = 100 + (i + 1) * 1;  // mm
        u_data[3] = 0;
        u_data[4] = 0;
        u_data[5] = 0;
        u_data[6] = 100;  // ticks

        for (float value : u_data) {
            out << value; // 流式写入，自动处理二进制编码
        }
    }

    u_data[0] = 2;
    u_data[1] = 20;  // mm
    u_data[2] = 20;  // mm
    u_data[3] = 0;
    u_data[4] = 0;
    u_data[5] = 0;
    u_data[6] = 0;  // ticks

    for (float value : u_data) {
        out << value; // 流式写入，自动处理二进制编码
    }


    data_file.close();

    return true;
}


bool robot_trace::trace_to_controller(ZMC_HANDLE g_handle, const QString& cus_file_name)
{
    // ---------- Set Up ----------
    QString file_name = cus_file_name.isEmpty() ? "01" : cus_file_name;
    QString file_path = QString("%1%2.dat").arg(data_dir, file_name);
    QFile data_file(file_path);


    if (!data_file.open(QIODevice::ReadOnly)) {  // 注意写的时候判定是Write
        qWarning() << "无法打开文件：" << file_path;
        qWarning() << "错误信息：" << data_file.errorString();
        return false;
    }
    qDebug() << "trace file open sucess.";

    QDataStream in(&data_file);
    // 设置字节序为小端，保证跨平台一致性（也可用QDataStream::BigEndian）
    in.setByteOrder(QDataStream::LittleEndian);
    // 设置双精度浮点数精度为标准32位
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);


    ZAux_Trigger(g_handle);

    uint16 data_state;
    data_state = F_DataBlank;
    int loop_num = 0;  // 数据块发送次数
    int cur_group_id = 0;  // 当前应发送的数据块id
    int cur_tabel_index;  // 当前table数组起始操作的下标
    float data_list[DataBlockSize];  // 待发送的数据列表
    const int block_size_bytes = DataBlockSize * sizeof(float);

    // ---------- Command Delivery ----------
    while(!in.atEnd())
    {
        cur_group_id = loop_num % DataGroupNum;  // 取余
        // 查看控制器中的数据状态，只要不是Updata就可以下发
        do
        {
            ZAux_Modbus_Get4x(g_handle, cur_group_id, 1, &data_state);
        }while(!(data_state != F_DataUpdate));

        //数据获取
        if (!data_file.seek(cur_group_id * block_size_bytes))
        {
            return false;
        }

        for(int i_file = 0; i_file < DataBlockSize; i_file++)
        {
            if(!in.atEnd())
            {
                in >> data_list[i_file];
            }
            else
            {
                // 都给M(0)
                if((i_file % CmdSize) != 0)
                {
                    data_list[i_file] = 0;
                }
                else
                {
                    data_list[i_file] = 1;
                }
                // 若在文件末尾，则读上一组数据，使其保持在原位
                // if(i_file < CmdSize)
                // {
                //     if(loop_num != 0)
                //     {
                //         data_list[i_file] = data_list[i_file + DataBlockSize -CmdSize];
                //     }
                //     else
                //     {
                //         data_list[i_file] = 0;  // 如果是第一次读入，就赋值0
                //     }
                // }
                // else
                // {
                //     data_list[i_file] = data_list[i_file -CmdSize];
                // }
            }
        }


        // 数据发送
        cur_tabel_index = DataStart_Index + cur_group_id * DataBlockSize;
        ZAux_Direct_SetTable(g_handle, cur_tabel_index, DataBlockSize, data_list);

        // 数据状态更新
        data_state = F_DataUpdate;
        ZAux_Modbus_Set4x(g_handle, cur_group_id, 1, &data_state);

        loop_num++;

    }


    data_file.close();
    qDebug() << "motion command deliver over.";
    return true;
}



bool robot_trace::xlsx_init(const QString& cus_file_name)
{
    QString file_name = cus_file_name.isEmpty() ? "01" : cus_file_name;
    QString file_path = QString("%1%2.xlsx").arg(data_dir, file_name);

    if(!p_xlsx)
    {
        p_xlsx = new QXlsx::Document(file_path, this);
        p_xlsx->write(1,8, QDateTime::currentDateTime());
    }

    if(p_xlsx->saveAs(file_path))
    {
        qInfo() << "Excel文件创建成功:" << file_path;
        return true;
    }
    else
    {
        qWarning() << "Excel文件创建失败:" << file_path;
        delete p_xlsx;
        p_xlsx = nullptr;
        return false;
    }
    return true;
}



bool robot_trace::xlsx_close()
{
    if(p_xlsx)
    {
        p_xlsx->save();
        delete p_xlsx;
        p_xlsx = nullptr;
    }
    return true;
}


bool robot_trace::dat_to_xlsx(const QString& dat_file_name, const QString& xlsx_file_name)
{
    xlsx_init(xlsx_file_name);

    QString file_name = dat_file_name.isEmpty() ? "01" : dat_file_name;
    QString file_path = QString("%1%2.dat").arg(data_dir, file_name);
    QFile data_file(file_path);

    if (!data_file.open(QIODevice::ReadOnly)) {  // 注意写的时候判定是Write
        qWarning() << "无法打开文件：" << file_path;
        qWarning() << "错误信息：" << data_file.errorString();
        return false;
    }

    QDataStream in(&data_file);
    in.setByteOrder(QDataStream::LittleEndian);  // 设置字节序为小端，保证跨平台一致性（也可用QDataStream::BigEndian）
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);  // 设置双精度浮点数精度为标准32位

    int loop_num = 0;  // 数据块发送次数
    float data_list[DataBlockSize];  // 待发送的数据列表
    const int block_size_bytes = DataBlockSize * sizeof(float);

    // ---------- Command Delivery ----------
    while(!in.atEnd())
    {

        //数据获取
        if (!data_file.seek(loop_num * block_size_bytes))  // 用于移动指针位置，注意移动的大小
        {
            return false;
        }

        for(int i_file = 0; i_file < DataBlockSize; i_file++)
        {
            if(!in.atEnd())
            {
                in >> data_list[i_file];
            }
            else
            {   // 空指令赋值MOVE(0)
                if(i_file % CmdSize)
                {
                    data_list[i_file] = 0;
                }
                else
                {
                    data_list[i_file] = 1;
                }
                // if(i_file < CmdSize)
                // {
                //     if(loop_num != 0)
                //     {
                //         data_list[i_file] = data_list[i_file + DataBlockSize - CmdSize];
                //     }
                //     else
                //     {
                //         data_list[i_file] = 0;  // 如果是第一次读入，就赋值0
                //     }
                // }
                // else
                // {
                //     data_list[i_file] = data_list[i_file - CmdSize];
                // }
            }
        }

        // 写入EXCEL
        int line_id;
        int col_id;

        for(int i_xlsx = 0; i_xlsx < DataBlockSize; i_xlsx++)
        {
            line_id = i_xlsx /CmdSize + DataGroupSize * loop_num + 1;
            col_id = i_xlsx %CmdSize + 1;
            p_xlsx->write(line_id, col_id, data_list[i_xlsx]);
        }

        loop_num++;
        qDebug() << "loop_num = " << loop_num;

    }

    xlsx_close();
    data_file.close();
    return true;
}


bool robot_trace::xlsx_to_dat(const QString& dat_file_name, const QString& xlsx_file_name)
{
    xlsx_init(xlsx_file_name);

    QString file_name = dat_file_name.isEmpty() ? "01" : dat_file_name;
    QString file_path = QString("%1%2.dat").arg(data_dir, file_name);
    QFile data_file(file_path);

    if (!data_file.open(QIODevice::WriteOnly)) {  // 注意写的时候判定是Write
        qWarning() << "无法打开文件：" << file_path;
        qWarning() << "错误信息：" << data_file.errorString();
        return false;
    }

    QDataStream out(&data_file);
    // 设置字节序为小端，保证跨平台一致性（也可用QDataStream::BigEndian）
    out.setByteOrder(QDataStream::LittleEndian);
    // 设置双精度浮点数精度为标准32位
    out.setFloatingPointPrecision(QDataStream::SinglePrecision);


    if(p_xlsx)
    {
        int rowlen = p_xlsx->dimension().rowCount();
        float u_data[CmdSize];
        for(int i_row = 0; i_row < rowlen; i_row++)
        {
            for(int i_col = 0; i_col <CmdSize; i_col++)
            {
                u_data[i_col] = p_xlsx->read(i_row + 1, i_col + 1).toFloat();  // ***Excel和C下标的差异***
            }


            for (float value : u_data)
            {
                out << value; // 流式写入，自动处理二进制编码
            }

        }
    }

    xlsx_close();
    data_file.close();
    return true;

}

