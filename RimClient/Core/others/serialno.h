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

#include <QQueue>
#include <QWaitCondition>

#include "Network/rtask.h"

class SerialNo : public ClientNetwork::RTask
{
    Q_OBJECT
public:
    SerialNo();
    static SerialNo* instance();
    unsigned int FrashSerialNo();
    unsigned int SetSerialNo(unsigned int No);

    void startMe();
    void stopMe();

    enum SerialTaskType
    {
        PushSerialNo,   //更新数据库中流水号
        PullSerialNo    //获取最新流水号
    };
    struct SerialTask{
        SerialTask() : taskType(PullSerialNo),No(0)
        {}
        SerialTaskType taskType;
        unsigned short No;
    };

    void addPushTask(unsigned short No);
    void addPullTask();

    unsigned short getSerialNo();
    bool updateSerialNo( unsigned short No);
private:

    QWaitCondition runWaitCondition;
    QMutex m_Pause;
    static SerialNo * staic_SerialNo;
};

#endif // SERIALNO_H
