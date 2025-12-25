#include "robo_trace.h"
#include <QApplication>

#define PI 3.14159
#define F_DataUpdate (uint16)1
#define F_DataUsed (uint16)2
#define F_DataBlank (uint16)3
#define DataStart_Index 1000
#define DataGroupSize 100  // 数据块中的数据组数
#define GroupSize 6  // 每组数据大小
#define DataGroupNum 10  // 数据块缓冲的块数
#define DataBlockSize  DataGroupSize*GroupSize  // 每次发送的数据总数

robo_trace::robo_trace()
{
    data_dir = "../../trace_data/";
}


void robo_trace::run()
{

}


bool robo_trace::trace_to_file1(const QString& cus_file_name)
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

    data_file.close();

    return true;
}


bool robo_trace::trace_read(const QString& cus_file_name)
{
    QString file_name = cus_file_name.isEmpty() ? "01" : cus_file_name;
    QString file_path = QString("%1%2.dat").arg(data_dir, file_name);


    QFile data_file(file_path);


    if (!data_file.open(QIODevice::ReadOnly)) {  // 注意写的时候判定是Write
        qWarning() << "无法打开文件：" << file_path;
        qWarning() << "错误信息：" << data_file.errorString();
        return false;
    }

    QDataStream in(&data_file);
    // 设置字节序为小端，保证跨平台一致性（也可用QDataStream::BigEndian）
    in.setByteOrder(QDataStream::LittleEndian);
    // 设置双精度浮点数精度为标准32位
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    for (int i = 0; i < 100 && !in.atEnd(); ++i) {
        QVector<float> group(6);
        for (int j = 0; j < 6; ++j) {
            in >> group[j]; // 流式读取
        }
        qDebug() << group[1];
    }


    data_file.close();
    return true;
}


bool robo_trace::trace_to_controller(ZMC_HANDLE g_handle, const QString& cus_file_name)
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
    float pos_list[DataBlockSize];  // 待发送的数据列表
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
                in >> pos_list[i_file];
            }else
            {  // 若在文件末尾，则读上一组数据，使其保持在原位
                if(i_file < GroupSize)
                {
                    if(loop_num != 0)
                    {
                        pos_list[i_file] = pos_list[i_file + DataBlockSize - GroupSize];
                    }
                    else
                    {
                        pos_list[i_file] = 0;  // 如果是第一次读入，就赋值0
                    }
                }
                else
                {
                    pos_list[i_file] = pos_list[i_file - GroupSize];
                }
            }
        }


        // 数据发送
        cur_tabel_index = DataStart_Index + cur_group_id * DataBlockSize;
        ZAux_Direct_SetTable(g_handle, cur_tabel_index, DataBlockSize, pos_list);

        // 数据状态更新
        data_state = F_DataUpdate;
        ZAux_Modbus_Set4x(g_handle, cur_group_id, 1, &data_state);

        loop_num++;

    }


    data_file.close();
    return true;
}

bool robo_trace::xlsx_init(const QString& cus_file_name)
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


bool robo_trace::xlsx_close()
{
    if(p_xlsx)
    {
        p_xlsx->save();
        delete p_xlsx;
        p_xlsx = nullptr;
    }
    return true;
}


bool robo_trace::trace_to_xlsx(const QString& dat_file_name, const QString& xlsx_file_name)
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
    float pos_list[DataBlockSize];  // 待发送的数据列表
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
                in >> pos_list[i_file];
            }
            else
            {  // 若在文件末尾，则读上一组数据，使其保持在原位
                if(i_file < GroupSize)
                {
                    if(loop_num != 0)
                    {
                        pos_list[i_file] = pos_list[i_file + DataBlockSize - GroupSize];
                    }
                    else
                    {
                        pos_list[i_file] = 0;  // 如果是第一次读入，就赋值0
                    }
                }
                else
                {
                    pos_list[i_file] = pos_list[i_file - GroupSize];
                }
            }
        }

        // 写入EXCEL
        int line_id;
        int col_id;

        for(int i_xlsx = 0; i_xlsx < DataBlockSize; i_xlsx++)
        {
            line_id = i_xlsx / GroupSize + DataGroupSize * loop_num + 1;
            col_id = i_xlsx % GroupSize + 1;
            p_xlsx->write(line_id, col_id, pos_list[i_xlsx]);
        }

        loop_num++;
        qDebug() << "loop_num = " << loop_num;

    }

    xlsx_close();
    data_file.close();
    return true;
}


bool robo_trace::xlsx_to_dat(const QString& dat_file_name, const QString& xlsx_file_name)
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
        float u_data[GroupSize];
        for(int i_row = 0; i_row < rowlen; i_row++)
        {
            for(int i_col = 0; i_col < GroupSize; i_col++)
            {
                u_data[i_col] = p_xlsx->read(i_row + 1, i_col + 1).toFloat();  // ***Excel和C下标的差异***
            }


            for (float value : u_data) {
                out << value; // 流式写入，自动处理二进制编码
            }

        }
    }


    xlsx_close();
    data_file.close();
    return true;

}





















