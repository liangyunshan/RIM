/*!
 *  @brief     自动事务处理
 *  @details   提供自动事务处理(若支持)操作，默认自动开启事务，自动提交事务，出错可回滚。简化在代码中多次手动操作事务;
 *             [1]此功能适用于在一个函数内的事务处理，跨函数执行需要传递对应的databse连接,确保在同一事务中
 *             [2]若不支持事务，默认影响操作结果，只是确保对应的保护
 *  @author    wey
 *  @version   1.0
 *  @date      2018.04.28
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef AUTOTRANSACTION_H
#define AUTOTRANSACTION_H

#include <QSqlDatabase>

#define BeginTransaction(database) AutoTransaction autoT(database)
#define RollBackTransaction (autoT(true))
#define ThrowLine (throw __LINE__)

class AutoTransaction
{
public:
    explicit AutoTransaction(QSqlDatabase & db);
    ~AutoTransaction();
    AutoTransaction(const AutoTransaction & db) = delete;
    AutoTransaction operator=(const AutoTransaction& db ) = delete;

    void operator()(bool flag);

private:
    QSqlDatabase database;
    bool rollBack;
    bool transacting;
};

#endif // AUTOTRANSACTION_H
