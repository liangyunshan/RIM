/*!
 *  @brief     流水号更新类
 *  @details   连接数据库，实时更新流水号
 *  @author    尚超
 *  @version   1.0
 *  @date      2018.07.17
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef SERIALNO_H
#define SERIALNO_H

#include <mutex>
#include <condition_variable>

class SerialNo
{
public:
    SerialNo();

    static SerialNo* instance();

    unsigned short FrashSerialNo();
    void SetSerialNo(unsigned short No);
    unsigned short getSqlSerialNo();
    bool updateSqlSerialNo( unsigned short No);

private:
    unsigned short m_SerialNo;
    static SerialNo * staic_SerialNo;
};

#endif // SERIALNO_H
