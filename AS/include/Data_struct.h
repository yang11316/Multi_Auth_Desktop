#ifndef DATA_STRUCT_H
    #define DATA_STRUCT_H

#endif // DATA_STRUCT_H

#include <QDataStream>
#include <iostream>
#include <string>
#include <QDebug>

#include "KGC_pariFree.h"
#include "Process_pairFree.h"
#include "accumulator.h"

struct Kgc_Parameter
{
    QString Ppub_hex;
    int k;

    // 重写QDataStream& operator<<操作符，做数据序列化操作
    friend QDataStream &operator<<(QDataStream &stream, const Kgc_Parameter &tmp_parameter)
    {
        // 将数据输入流对象中
        stream << tmp_parameter.Ppub_hex;
        stream << tmp_parameter.k;
        return stream;
    }

    // 重写QDataStream& operator>>操作符，做数据反序列化操作
    friend QDataStream &operator>>(QDataStream &stream, Kgc_Parameter &tmp_parameter)
    {
        // 从流对象中输出数据到学生结构体引用中
        stream >> tmp_parameter.Ppub_hex;
        stream >> tmp_parameter.k;
        return stream;
    }

    // 重写QDebug operator<<函数，用作输出内容
    friend QDebug operator<<(QDebug debug, const Kgc_Parameter &tmp_parametert)
    {
        // 向QDebug对象输入数据
        debug << "Ppub_hex " << tmp_parametert.Ppub_hex;
        debug << "\nk " << tmp_parametert.k;
        debug << "\n";
        return debug;
    }
};
