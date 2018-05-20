/*!
 *  @brief     历史聊天面板数据处理
 *  @details   对历史聊天记录的插入、更新、提取，减少主线程执行时的卡顿现象
 *  @author    wey
 *  @version   1.0
 *  @date      2018.05.15
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef HISTORYRECORDTASK_H
#define HISTORYRECORDTASK_H

#include "Network/rtask.h"

#include <condition_variable>
#include <mutex>
#include <functional>
#include <list>
#include <queue>

class HistoryRecordTask : public ClientNetwork::RTask
{
    Q_OBJECT
public:
    HistoryRecordTask(QObject *parent = Q_NULLPTR);
    ~HistoryRecordTask();

    static HistoryRecordTask * instance();

    void executeTask(std::function<bool()> func,std::function<void()> callBack = nullptr);

    void startMe();
    void stopMe();

protected:
    void run();

private:
    static HistoryRecordTask * hitoryTask;

    std::mutex fmutex;
    std::condition_variable cv;
    std::map<QString,std::pair<std::function<bool()>,std::function<void()>>> funcMap;
};

#endif // HISTORYRECORDTASK_H
